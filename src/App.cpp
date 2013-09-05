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
			else
			{
				Sleep(10);
			}

            mTimer.stop();

            d = mTimer.getTimeElapsed();
        }
    }

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
    mEntity.getSolidity().radius = 0.08f;
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
             rotVec = XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( camKeyDown[1] ){
             rotVec = XMVectorSet( 0.0f, 0.0f, -1.0f, 1.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( camKeyDown[2] ){
             rotVec = XMVectorSet( 1.0f, 0.0f, 0.0f, 1.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( camKeyDown[3] ){
             rotVec = XMVectorSet( -1.0f, 0.0f, 0.0f, 1.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }

        moveVec = XMVector4Normalize( moveVec );

        savePos = XMLoadFloat4( &mEntity.getPosition() );

        finalPos = savePos + XMVectorScale( moveVec, dt * 2.0f );

        XMStoreFloat4( &mEntity.getPosition(), finalPos );

        //Are we colliding?
        int startX = static_cast<int>(mEntity.getPosition().x / 0.3f);
        int startZ = static_cast<int>(mEntity.getPosition().z / 0.3f);

        CLAMP( startX, 0, mWorld.getEnv().getRoom().getWidth() - 1 );
        CLAMP( startZ, 0, mWorld.getEnv().getRoom().getDepth() - 1 );

        if( mWorld.getEnv().getRoom().getBlockRamp(startX, startZ) == Environment::Room::Block::RampDirection::None ){
            //Make sure we are on the floor, otherwise bring us down through gravity
            float distFromGround = ( mEntity.getPosition().y - mEntity.getSolidity().height ) - static_cast<float>(mWorld.getEnv().getRoom().getBlockHeight(startX, startZ)) * 0.3f;

            if( distFromGround > 0.05f ){
                mEntity.getPosition().y -= 0.045f;
            }
        }

        startX -= 1;
        startZ -= 1;

        int endX = startX + 2;
        int endZ = startZ + 2;

        CLAMP( startX, 0, mWorld.getEnv().getRoom().getWidth() - 1 );
        CLAMP( startZ, 0, mWorld.getEnv().getRoom().getDepth() - 1 );
        CLAMP( endX, 0, mWorld.getEnv().getRoom().getWidth() - 1 );
        CLAMP( endZ, 0, mWorld.getEnv().getRoom().getDepth() - 1 );

        bool collision = false;

        for(int i = startX; i <= endX; i++){
            for(int j = startZ; j <= endZ; j++){

                float blockHeight = static_cast<float>(mWorld.getEnv().getRoom().getBlockHeight(i, j));
                blockHeight *= 0.3f;

                float playerHeight = mEntity.getPosition().y - mEntity.getSolidity().height;
                float px = mEntity.getPosition().x;
                float pz = mEntity.getPosition().z;

                if(  blockHeight > playerHeight ){
                    //Check to see if any of the points on the block touch the player
                    float left = static_cast<float>(i) * 0.3f;
                    float top = static_cast<float>(j) * 0.3f;
                    float right = left + 0.3f;
                    float bottom = top + 0.3f;

                    //Left to Right in front
                    if( WorldEntity::circleLineIntersect( 
                        XMFLOAT2(left, top),
                        XMFLOAT2(right, top),
                        XMFLOAT2(px, pz),
                        mEntity.getSolidity().radius ) ){
                        sprintf(collidedString, "Collision!");
                        break;
                    }

                    //Left to Right in back
                    if( WorldEntity::circleLineIntersect( 
                        XMFLOAT2(left, bottom),
                        XMFLOAT2(right, bottom),
                        XMFLOAT2(px, pz),
                        mEntity.getSolidity().radius ) ){
                        sprintf(collidedString, "Collision!");
                        break;
                    }

                    //top to bottom on left
                    if( WorldEntity::circleLineIntersect( 
                        XMFLOAT2(left, top),
                        XMFLOAT2(left, bottom),
                        XMFLOAT2(px, pz),
                        mEntity.getSolidity().radius ) ){
                        sprintf(collidedString, "Collision!");
                        break;
                    }

                    //top to bottom on right
                    if( WorldEntity::circleLineIntersect( 
                        XMFLOAT2(right, top),
                        XMFLOAT2(right, bottom),
                        XMFLOAT2(px, pz),
                        mEntity.getSolidity().radius ) ){
                        sprintf(collidedString, "Collision!");
                        break;
                    }
                }
            }
        }

        mCamera.getPosition() = mEntity.getPosition();
    }else{
        if( camKeyDown[0] ){
            mCamera.moveForwardBack( 1.03f * dt ); 
        }
        
        if( camKeyDown[1] ){
            mCamera.moveForwardBack( -1.03f * dt ); 
        }
        
        if( camKeyDown[2] ){
            mCamera.moveLeftRight( 1.03f * dt ); 
        }
        
        if( camKeyDown[3] ){
            mCamera.moveLeftRight( -1.03f * dt ); 
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
}