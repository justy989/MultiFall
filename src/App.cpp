#include "App.h"
#include "Log.h"
#include "Utils.h"

#include "WorldGenerator.h"

App::App()
{
    camKeyDown[0] = false;
    camKeyDown[1] = false;
    camKeyDown[2] = false;
    camKeyDown[3] = false;

    wireframe = false;

    FPSString[0] = '\0';
    CameraPosString[0] = '\0';
}

void App::handleInput( RAWINPUT* input )
{
    if (input->header.dwType== RIM_TYPEMOUSE){
        //Mouse change since the last input received
        long mx = input->data.mouse.lLastX;
        long my = input->data.mouse.lLastY;

        //0.001f is hardcoded, but we should use a sensitivity configuration setting
        mCamera.modPitch( (float)(mx) * 0.001f );
        mCamera.modYaw( (float)(my) * -0.001f );

    }else if (input->header.dwType== RIM_TYPEKEYBOARD){
        USHORT VKey = input->data.keyboard.VKey;

        float camSpeed = 0.25f;

        bool keyUp = input->data.keyboard.Flags & RI_KEY_BREAK;

        switch( VKey ){
        case VK_SPACE:
            {
                WorldGenerator worldGen;
                
                worldGen.genRoom( mWorld.getEnv().getRoom() );
                mWorldDisplay.getEnvDis().createRoomMesh( mWindow.getDevice(), mWorld.getEnv().getRoom() );
            }
            break;
        case 'A':
            camKeyDown[0] = !keyUp;
            break;
        case 'D':
            camKeyDown[1] = !keyUp;
            break;
        case 'W':
            camKeyDown[2] = !keyUp;
            break;
        case 'S':
            camKeyDown[3] = !keyUp;
            break;
        case 'P': //Toggle wireframe
            {
                if( keyUp ){
                    break;
                }

                wireframe = !wireframe;

                if( wireframe ){
                    mWindow.getDeviceContext()->RSSetState( mWireFrameRS );
                }else{
                    mWindow.getDeviceContext()->RSSetState( mFillRS );
                }
            }
            break;
        default:
            break;
        }
    }
}

int App::run( HINSTANCE hInstance, int nCmdShow )
{
    MSG msg = {0};
    float fpsDelay = 0.0f;

    //How do we report this if there is no log?!?
    if( !Log::setFilePath("multifall.log") ){
        return 1;
    }

    if( !mWindow.init( L"MultiFall", 1280, 1024, hInstance, nCmdShow, this ) ){
        return 1;
    }

    if( !init() ){
        return 1;
    }
    
    mTimer.start();

    float d = 0.0f; // Delta time per frame

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
        {
            if( !mWindow.isPaused() /*&& mTimer.getTimeElapsed() > LIMIT_60_FPS*/ )
			{
                mTimer.start();

                if( camKeyDown[0] ){
                    mCamera.moveForwardBack( 1.03f * d );
                }else if( camKeyDown[1] ){
                    mCamera.moveForwardBack( -1.03f * d );
                }else if( camKeyDown[2] ){
                    mCamera.moveLeftRight( 1.03f * d );
                }else if( camKeyDown[3] ){
                    mCamera.moveLeftRight( -1.03f * d );
                }

                mCamera.update( mWindow.getAspectRatio() );

                fpsDelay += d;

                if( fpsDelay > 1.0f ){
                    sprintf(FPSString, "FPS: %.2f", 1.0f / d );
                    fpsDelay = 0.0f;
                }

                sprintf(CameraPosString, "Camera: %.2f, %.2f, %.2f", mCamera.getPosition().x, mCamera.getPosition().y, mCamera.getPosition().z );

				update();
                draw();
			}
			else
			{
				Sleep(10);
			}

            mTimer.stop();

            d = mTimer.getTimeElapsed();
        }
    }

    LOG_INFO << "Goodbye cruel world of MultiFall" << LOG_ENDL;
	return (int)msg.wParam;
}

bool App::init( )
{
    if( !mWorldDisplay.init( mWindow.getDevice() ) ){
        return false;
    }

	mTextManager.init(mWindow.getDevice());

    D3D11_RASTERIZER_DESC rasterDesc;

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    mWindow.getDevice()->CreateRasterizerState(&rasterDesc, &mWireFrameRS);

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    mWindow.getDevice()->CreateRasterizerState(&rasterDesc, &mFillRS);

    //TMP Stuff
    WorldGenerator worldGen;
    worldGen.genRoom( mWorld.getEnv().getRoom() );

    mWorldDisplay.getEnvDis().createRoomMesh( mWindow.getDevice(), mWorld.getEnv().getRoom() );	

    return true;
}

void App::update( )
{

}

void App::draw( )
{
    mWindow.clearBDS();

	// Set constants
    XMMATRIX view  = XMLoadFloat4x4(&mCamera.getView());
    XMMATRIX proj  = XMLoadFloat4x4(&mCamera.getProj());
	XMMATRIX viewProj = view * proj;

	ID3DX11EffectMatrixVariable* mfxViewProj = mWorldDisplay.getFX()->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxViewProj->SetMatrix(reinterpret_cast<float*>(&viewProj));

    mWorldDisplay.draw( mWindow.getDeviceContext(), mWorld );

    mTextManager.DrawString(mWindow.getDeviceContext(), FPSString, 0, 0);
    mTextManager.DrawString(mWindow.getDeviceContext(), CameraPosString, 0, 25);

    mWindow.getSwapChain()->Present(0, 0);
}

void App::clear( )
{
    mWorldDisplay.clear();
}