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
}

void App::handleInput( RAWINPUT* input )
{
    if (input->header.dwType== RIM_TYPEMOUSE){
        //Mouse position on window
        long mx = input->data.mouse.lLastX;
        long my = input->data.mouse.lLastY;

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
                mWorld.getEnv().getRoom().clear();
                mWorld.getEnv().getRoom().init( 16, 16, 6 );
                
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

    if( !mWindow.init( L"MultiFall", 1280, 1024, hInstance, nCmdShow, this ) ){
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

	return (int)msg.wParam;
}

bool App::init( )
{
    /*
    float color[3] = {1.0f, 1.0f, 1.0f};

    //Fill in vertex and index buffers
    EnvVertex vertices[] =
    {
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), color },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), color },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), color },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), color },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), color },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), color },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), color },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), color }
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(EnvVertex) * 8;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

    if(FAILED(mWindow.getDevice()->CreateBuffer(&vbd, &vinitData, &mEnvVB))){
        return false;
    }*/

	// Create the index buffer
    /*
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * 36;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    if(FAILED(mWindow.getDevice()->CreateBuffer(&ibd, &iinitData, &mEnvIB))){
        return false;
    }
    */

    if( !mWorldDisplay.init( mWindow.getDevice() ) ){
        return false;
    }

    mWorld.getEnv().getRoom().init( 16, 16, 6 );

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

    //Set vertex layout
	//mWindow.getDeviceConext()->IASetInputLayout(mInputLayout);
    //mWindow.getDeviceConext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Set VB, IA, Textures etc...

	// Set constants
	XMMATRIX world = XMMatrixTranslation( 0.3f, 0.3f, 0.0f );
    XMMATRIX view  = XMLoadFloat4x4(&mCamera.getView());
    XMMATRIX proj  = XMLoadFloat4x4(&mCamera.getProj());
	XMMATRIX ViewProj = world * view * proj;

	ID3DX11EffectMatrixVariable* mfxViewProj = mWorldDisplay.getFX()->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxViewProj->SetMatrix(reinterpret_cast<float*>(&ViewProj));

    D3DX11_TECHNIQUE_DESC techDesc;
    mWorldDisplay.getTechnique()->GetDesc( &techDesc );

    for(UINT p = 0; p < techDesc.Passes; ++p){
        mWorldDisplay.getTechnique()->GetPassByIndex(p)->Apply(0, mWindow.getDeviceContext());
        
		//Draw Indexed Primitives
        mWorldDisplay.draw( mWindow.getDeviceContext(), mWorld );
    }

    mWindow.getSwapChain()->Present(0, 0);
}

void App::clear( )
{

}