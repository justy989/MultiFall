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
    collisionMode = false;

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
        case 'R':
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
            }
            break;
        case 'O':

            if( keyUp ){
                break;
            }

            collisionMode = !collisionMode;

            if( collisionMode ){
                mEntity.getPosition() = mCamera.getPosition();
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

                mCamera.update( mWindow.getAspectRatio() );

                fpsDelay += d;

                if( fpsDelay > 1.0f ){
                    sprintf(FPSString, "FPS: %.2f", 1.0f / d );
                    fpsDelay = 0.0f;
                }

                sprintf(CameraPosString, "%s: %.2f, %.2f, %.2f", collisionMode ? "Player" : "Camera",  mCamera.getPosition().x, mCamera.getPosition().y, mCamera.getPosition().z );

                collidedString[0] = '\0';

				update( d );
                draw();
			}

            Sleep(10);

            mTimer.stop();

            d = mTimer.getTimeElapsed();
        }
    }

    clear();

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

    mEntity.getSolidity().type = WorldEntity::Solidity::BodyType::Cylinder;
    mEntity.getSolidity().radius = 0.15f;
    mEntity.getSolidity().height = 0.25f;

    return true;
}

void App::update( float dt )
{
    if( collisionMode ){

        XMVECTOR rotVec;
        XMMATRIX rotMat;
        XMVECTOR moveVec;
        XMVECTOR savePos;
        XMVECTOR finalPos;

        moveVec = XMVectorZero();
        rotMat = XMMatrixRotationAxis( XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f), mCamera.getPitch() );
             
        if( camKeyDown[0] ){
             rotVec = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( camKeyDown[1] ){
             rotVec = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( camKeyDown[2] ){
             rotVec = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( camKeyDown[3] ){
             rotVec = XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }

        moveVec = XMVector3Normalize( moveVec );

        moveVec = XMVectorSetW( moveVec, 1.0f );

        //TEMPORARY WAY OF SOLVING THIS!
        //As long as we are not on a ramp, bring us to the floor
        int bx = static_cast<int>( mEntity.getPosition().x / 0.3f);
        int bz = static_cast<int>( mEntity.getPosition().z / 0.3f);

        CLAMP( bx, 0, mWorld.getEnv().getRoom().getWidth() - 1 );
        CLAMP( bz, 0, mWorld.getEnv().getRoom().getDepth() - 1 );

        if( mWorld.getEnv().getRoom().getBlockRamp(bx, bz) == Environment::Room::Block::RampDirection::None ){
            //Make sure we are on the floor, otherwise bring us down through gravity
            float distFromGround = ( mEntity.getPosition().y - mEntity.getSolidity().height ) - static_cast<float>(mWorld.getEnv().getRoom().getBlockHeight(bx, bz)) * 0.3f;

            if( distFromGround > 0.05f ){
                mEntity.getPosition().y -= 0.045f;
            }else if( distFromGround < -0.05f ){
                //mEntity.getPosition().y += 0.045f;
            }
        }

        mWorld.moveEntity( &mEntity, moveVec, dt );

        mCamera.getPosition() = mEntity.getPosition();
    }else{
        if( camKeyDown[0] ){
            mCamera.moveForwardBack( 1.0f * dt ); 
        }
        
        if( camKeyDown[1] ){
            mCamera.moveForwardBack( -1.0f * dt ); 
        }
        
        if( camKeyDown[2] ){
            mCamera.moveLeftRight( 1.0f * dt ); 
        }
        
        if( camKeyDown[3] ){
            mCamera.moveLeftRight( -1.0f * dt ); 
        }
    }

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
    
    if( wireframe ){
        mWindow.getDeviceContext()->RSSetState( mWireFrameRS );
    }else{
        mWindow.getDeviceContext()->RSSetState( mFillRS );
    }

    mWorldDisplay.draw( mWindow.getDeviceContext(), mWorld );

    mWindow.getDeviceContext()->RSSetState( mFillRS );

    mTextManager.DrawString(mWindow.getDeviceContext(), FPSString, 0, 0);
    mTextManager.DrawString(mWindow.getDeviceContext(), CameraPosString, 0, 25);
    mTextManager.DrawString(mWindow.getDeviceContext(), collidedString, 0, 50);

    mWindow.getSwapChain()->Present(0, 0);
}

void App::clear( )
{
    mWorldDisplay.clear();
    mTextManager.clear();

    ReleaseCOM( mFillRS );
    ReleaseCOM( mWireFrameRS );

    mWindow.clear();
}