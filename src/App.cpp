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

    //How do we report this if there is no log?!?
    if( !Log::setFilePath("multifall.log") ){
        return 1;
    }

    if( !mWindow.init( L"MultiFall", 800, 600, hInstance, nCmdShow, this ) ){
        return 1;
    }

    if( !init() ){
        return 1;
    }
    
    mTimer.start();

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
            mTimer.stop();

            if( !mWindow.isPaused() && mTimer.getTimeElapsed() > LIMIT_60_FPS )
			{
                if( camKeyDown[0] ){
                    mCamera.moveForwardBack( 0.03f );
                }else if( camKeyDown[1] ){
                    mCamera.moveForwardBack( -0.03f );
                }else if( camKeyDown[2] ){
                    mCamera.moveLeftRight( 0.03f );
                }else if( camKeyDown[3] ){
                    mCamera.moveLeftRight( -0.03f );
                }

                mCamera.update( mWindow.getAspectRatio() );

				update();	
				draw();

                mTimer.start();
			}
			else
			{
				Sleep(10);
			}
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
    mWindow.getSwapChain()->Present(0, 0);
}

void App::clear( )
{
    mWorldDisplay.clear();
}