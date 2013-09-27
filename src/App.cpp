#include "App.h"
#include "Log.h"
#include "Utils.h"

#include "LevelThemeLoader.h"

char* gDropDownOptions[] = {
    "Red",
    "Green",
    "Blue"
};

App::App()
{
    camKeyDown[0] = false;
    camKeyDown[1] = false;
    camKeyDown[2] = false;
    camKeyDown[3] = false;

    collisionMode = false;
    freeLook = true;
    drawUI = false;

    FPSString[0] = '\0';
    CameraPosString[0] = '\0';
    MousePosString[0] = '\0';

    mLeftClick = false;
}

void App::handleInput( RAWINPUT* input )
{
    if (input->header.dwType== RIM_TYPEMOUSE){
        //Mouse change since the last input received
        long mx = input->data.mouse.lLastX;
        long my = input->data.mouse.lLastY;

        if( freeLook ){
            mCamera.modPitch( (float)(mx) * mConfig.getSensitivity() );
            mCamera.modYaw( (float)(my) * -mConfig.getSensitivity() );
        }

        if( input->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN ){
            mLeftClick = true;
        }else if( input->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP ){
            mLeftClick = false;
        }

    }else if (input->header.dwType== RIM_TYPEKEYBOARD){
        USHORT VKey = input->data.keyboard.VKey;
        bool keyUp = input->data.keyboard.Flags & RI_KEY_BREAK;

        mKeyPress = !keyUp;
        mKeyChar = VKey;

        switch( VKey ){
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
        case 'U':
            if( keyUp ){
                break;
            }

            drawUI = !drawUI;

            break;
        case 'R':
            {
                if( keyUp ){
                    break;
                }

                mWorldGen.genLevel( mWorld.getLevel(), mLevelPreset );
                mWorldDisplay.getLevelDisplay().createMeshFromLevel( mWindow.getDevice(), mWorld.getLevel(), 0.3f, 0.3f);
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
        case 'I':
            if( keyUp ){
                break;
            }

            freeLook = !freeLook;

            break;
        default:
            break;
        }
    }

    mBinds.updateKeyStates( input );
}

int App::run( HINSTANCE hInstance, int nCmdShow )
{
    MSG msg = {0};
    float fpsDelay = 0.0f;	

    //Load the configuration
    if( !mConfig.load( "content/multifall.cfg" ) ){
        return 1;
    }

    //Set the binds for movement for now
    for(int i = UserBinds::Action::Player_Move_Forward; i <= UserBinds::Action::Player_Move_Right; i++){
        mBinds.bindKey( (UserBinds::Action)(i), 
                        mConfig.getBinds()[ i ] );
    }

    if( !mConfig.getWindow().hardwareRendered ){
        mWindow.setDriverType( D3D_DRIVER_TYPE_REFERENCE );
    }

    if( !mWindow.init( L"MultiFall", mConfig.getWindow().width, mConfig.getWindow().height, hInstance, nCmdShow, this ) ){
        return 1;
    }    

	//so tired of stuff being dependent on this
	if( !initShaders() ){
        return 1;
    }

    if( !init() ){
        return 1;
    }

	if( !initRTVs() ){
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
	if( !mWorldDisplay.init( mWindow.getDevice(), mRenderGBufferTech ) ){
        return false;
    }	

	mTextManager.init(mWindow.getDevice());

    //TMP Stuff
    mLevelPreset.roomCount.min = 8;
    mLevelPreset.roomCount.max = 16;

    //mLevelPreset.roomWidth.min = 2;
    //mLevelPreset.roomWidth.max = 16;

    //mLevelPreset.roomDepth.min = 2;
    //mLevelPreset.roomDepth.max = 16;

    mLevelPreset.roomCeilingHeight.min = 3;
    mLevelPreset.roomCeilingHeight.max = 6;

    mLevelPreset.doorScrubChance.min = 0.0f;
    mLevelPreset.doorScrubChance.max = 1.0f;

    mLevelPreset.roomChances[ WorldGenerator::Room::Type::Empty ] = 0.1f;
    mLevelPreset.roomChances[ WorldGenerator::Room::Type::Labyrinth ] = 0.25f;
    mLevelPreset.roomChances[ WorldGenerator::Room::Type::BedRoom ] = 0.4f;
    mLevelPreset.roomChances[ WorldGenerator::Room::Type::Study ] = 0.5f;
    mLevelPreset.roomChances[ WorldGenerator::Room::Type::Library ] = 0.6f;
    mLevelPreset.roomChances[ WorldGenerator::Room::Type::Storage ] = 0.8f;
    mLevelPreset.roomChances[ WorldGenerator::Room::Type::DiningRoom ] = 0.9f;
    mLevelPreset.roomChances[ WorldGenerator::Room::Type::Ballroom ] = 1.0f;

    mEntity.getSolidity().type = WorldEntity::Solidity::BodyType::Cylinder;
    mEntity.getSolidity().radius = 0.15f;
    mEntity.getSolidity().height = 0.25f;

    if( !mUIDisplay.init( mWindow.getDevice(), L"content/textures/multifall_ui.png", L"content/shaders/ui.fx" ) ){
        return false;
    }

    //Set furniture dimensions based on bounding boxes 
    for(int i = 1; i < LEVEL_FURNITURE_TYPE_COUNT; i++){
        XMFLOAT3 dimensions;

        dimensions.x = mWorldDisplay.getLevelDisplay().getFurnitureMesh( (Level::Furniture::Type)(i) ).getBoundingBoxMax().x - 
                       mWorldDisplay.getLevelDisplay().getFurnitureMesh( (Level::Furniture::Type)(i) ).getBoundingBoxMin().x;
        dimensions.y = mWorldDisplay.getLevelDisplay().getFurnitureMesh( (Level::Furniture::Type)(i) ).getBoundingBoxMax().y - 
                       mWorldDisplay.getLevelDisplay().getFurnitureMesh( (Level::Furniture::Type)(i) ).getBoundingBoxMin().y;
        dimensions.z = mWorldDisplay.getLevelDisplay().getFurnitureMesh( (Level::Furniture::Type)(i) ).getBoundingBoxMax().z - 
                       mWorldDisplay.getLevelDisplay().getFurnitureMesh( (Level::Furniture::Type)(i) ).getBoundingBoxMin().z;

        //Scale the dimensions
        dimensions.x *= mWorldDisplay.getLevelDisplay().getFurnitureScale( (Level::Furniture::Type)(i) );
        dimensions.y *= mWorldDisplay.getLevelDisplay().getFurnitureScale( (Level::Furniture::Type)(i) );
        dimensions.z *= mWorldDisplay.getLevelDisplay().getFurnitureScale( (Level::Furniture::Type)(i) );

        mWorld.getLevel().setFurnitureDimensions( (Level::Furniture::Type)(i), dimensions );
    }

    UIWindow::Text text;
    text.message = "Window Title";
    text.offset.x = 0.0f;
    text.offset.y = 0.0f;

    mUIWindow.setPosition( XMFLOAT2( -0.75f, -0.75f ) );
    mUIWindow.setDimension( XMFLOAT2( 1.0f, 1.5f ) );
    mUIWindow.setText( text );

    mUIWindow.setMinDimensions( XMFLOAT2( 1.0f, 1.5f ) );

    mUIWindow.init( 3 );
    mUIWindow.initTab( 0, "Generator", 2 );
    mUIWindow.initTab( 1, "Settings", 2 );
    mUIWindow.initTab( 2, "Fog", 2 );

    UIButton* btn = new UIButton();

    XMFLOAT2 pos(0.1f, 0.3f);
    XMFLOAT2 dim(0.3f, 0.1f);

    btn->setPosition( pos );
    btn->setDimension( dim );

    text.message = "Gen";

    mUIWindow.addElem( btn, 0 );

    btn->setText(text);
    
    mUIWindow.setCurrentTab( 0 );

    mSlider = new UISlider();
    
    pos.x += 0.2f;

    mSlider->setPosition( pos );

    dim.x += 0.2f;

    mSlider->setDimension( dim );

    text.message = "FOV";
    text.offset.x = -8.0f * FONTWIDTH;
    text.offset.y = 0.0f;

    mSlider->setPercent( 0.5f );
    mSlider->setText( text );

    mUIWindow.addElem( mSlider, 1 );

    mCheckBox = new UICheckbox();

    text.message = "Draw Stats";
    text.offset.x = -7.0f * FONTWIDTH;

    mCheckBox->setText( text );

    pos.y += 0.15f;
    pos.x += 0.25f;

    mCheckBox->setPosition( pos );

    dim.x = 0.1f;
    dim.y = 0.1f;

    mCheckBox->setDimension( dim );

    mUIWindow.addElem( mCheckBox, 1 );

    mCheckBox->setChecked(true);

    mDropBox = new UIDropMenu();

    text.message = "Fog Color";
    text.offset.x = -10.0f * FONTWIDTH;

    mDropBox->setText( text );

    pos.y = 0.25f;
    pos.x = 0.5f;

    mDropBox->setPosition( pos );

    dim.x = 0.4f;
    dim.y = 0.1f;

    mDropBox->setDimension( dim );

    mDropBox->setSelectedOption(0);
    mDropBox->setOptions( gDropDownOptions, 3 );

    mUIWindow.addElem( mDropBox, 2 );

    mInputBox = new UIInputBox();

    text.message = "Name:";
    text.offset.x = -7.0f * FONTWIDTH;

    mInputBox->setText( text );

    pos.y = 0.45f;
    pos.x = 0.3f;

    mInputBox->setPosition( pos );

    dim.x = 0.4f;
    dim.y = 0.1f;

    mInputBox->setDimension( dim );

    mUIWindow.addElem( mInputBox, 0 );

    mTextBox = new UITextBox();

    pos.y = 0.4f;
    pos.x = 0.2f;

    mTextBox->setPosition( pos );

    dim.x = 0.7f;
    dim.y = 1.0f;

    mTextBox->setDimension( dim );

    char* textBoxText = "Now, this is a story all about how "
                 "my life got flipped-turned upside down "
                 "and I'd like to take a minute "
                 "just sit right there "
                 "I'll tell you how I became the prince of a town called Bel Air";

    mTextBox->setText( textBoxText, (uint)strlen( textBoxText ) );

    mUIWindow.addElem( mTextBox, 2 );

    D3D11_BUFFER_DESC constDesc;
    ZeroMemory( &constDesc, sizeof( constDesc ) );
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof( XMMATRIX ) * 2;
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    
    if( FAILED( mWindow.getDevice()->CreateBuffer( &constDesc, 0, &mPerObjectCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer" << LOG_ENDL;
        return false;
    }

    LevelThemeLoader ltl;

	ltl.loadTheme("content/themes/brick_wood.txt", mWindow.getDevice(), &mWorldGen, &mWorldDisplay.getLevelDisplay());

    mWorldGen.genLevel( mWorld.getLevel(), mLevelPreset );
    mWorldDisplay.getLevelDisplay().createMeshFromLevel( mWindow.getDevice(), mWorld.getLevel(), 0.3f, 0.3f );	

    return true;
}

bool App::initShaders()
{
	//Compile shader
    DWORD shaderFlags = 0;
//#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
//#endif
 
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"content/shaders/RenderGBuffer.hlsl", 0, 0, 0, "fx_5_0", shaderFlags, 
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if( compilationMsgs != 0 )
	{
		LOG_ERRO << (char*)compilationMsgs->GetBufferPointer() << LOG_ENDL;
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if(FAILED(hr))
	{
        LOG_ERRO << "D3DCompileFromFile() failed" << LOG_ENDL;
        return false;
	}

	if(FAILED( D3DX11CreateEffectFromMemory( compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 
        0,  mWindow.getDevice(), &mFX) ) ){
        LOG_ERRO << "CreateEffectFromMemory() Failed" << LOG_ENDL;
        return false;
    }

	// Done with compiled shader.
	ReleaseCOM(compiledShader);
	mRenderGBufferTech = mFX->GetTechniqueByName("GeoPass");
	mAmbientLightTech = mFX->GetTechniqueByName("AmbientLight");
	mPointLightTech = mFX->GetTechniqueByName("PointLight");
	mLightParticleTech = mFX->GetTechniqueByName("LightParticle");

    LOG_INFO << "Loaded color.fx shader Successfully" << LOG_ENDL;

	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//Create the input layout
    D3DX11_PASS_DESC passDesc;
    mAmbientLightTech->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(mWindow.getDevice()->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Environment Vertex Input Layout" << LOG_ENDL;
            return false;
    }

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory( &resourceData, sizeof( resourceData ) );

	EnvVertex fsQuad[] = {
		{ XMFLOAT3(-1, 1, 0.0511f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0,0) },
		{ XMFLOAT3(1, 1, 0.0511f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1,0) },
		{ XMFLOAT3(-1, -1, 0.0511f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0,1) },
		{ XMFLOAT3(1, -1, 0.0511f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1,1)}	
	};

	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(EnvVertex) * 4;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    resourceData.pSysMem = fsQuad;

    mWindow.getDevice()->CreateBuffer(&vbd, &resourceData, &mFSQuadVB);

	//clockwise
	WORD indices[] =
	{
		0,  1,  2,  
		1,  3,  2
	};

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory( &indexDesc, sizeof( indexDesc ) );
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof( WORD ) * 6;
	indexDesc.CPUAccessFlags = 0;
	resourceData.pSysMem = indices;
    
	mWindow.getDevice()->CreateBuffer( &indexDesc, &resourceData, &mFSQuadIB );

	return true;
}

bool App::initRTVs()
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = mConfig.getWindow().width;
	textureDesc.Height = mConfig.getWindow().height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = mWindow.getDevice()->CreateTexture2D(&textureDesc, NULL, &mGBufferTextures[0]);
	if(FAILED(result))
	{
		return false;
	}
	result = mWindow.getDevice()->CreateTexture2D(&textureDesc, NULL, &mGBufferTextures[1]);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for(int i = 0; i < 2; i++)
	{
		// Create the render target view.
		result = mWindow.getDevice()->CreateRenderTargetView(mGBufferTextures[i], &renderTargetViewDesc, &mGBufferRTVs[i]);
		if(FAILED(result))
		{
			return false;
		}

		// Create the shader resource view.
		result = mWindow.getDevice()->CreateShaderResourceView(mGBufferTextures[i], &shaderResourceViewDesc, &mGBufferSRVs[i]);
		if(FAILED(result))
		{
			return false;
		}
	}

	// Create the depth/stencil buffer and view.	
	textureDesc.Format     = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.BindFlags  = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	if(FAILED(result = mWindow.getDevice()->CreateTexture2D(&textureDesc, 0, &mGBufferTextures[2]))){
        LOG_ERRO << "Failed to CreateTexture2D() of DepthStencil" << LOG_ENDL;
        return false;
    }

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	if(FAILED(result = mWindow.getDevice()->CreateDepthStencilView(mGBufferTextures[2], &descDSV, &mDepthStencilView))){
        LOG_ERRO << "Failed to CreateDepthStencilView from device" << LOG_ENDL;
        return false;
    }


	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	// Create the shader resource view.
	result = mWindow.getDevice()->CreateShaderResourceView(mGBufferTextures[2], &shaderResourceViewDesc, &mGBufferSRVs[2]);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void App::update( float dt )
{
    if( collisionMode ){

        XMVECTOR rotVec;
        XMMATRIX rotMat;
        XMVECTOR moveVec;

        moveVec = XMVectorZero();
        rotMat = XMMatrixRotationAxis( XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f), mCamera.getPitch() );
             
        if( mBinds.isBindDown( UserBinds::Action::Player_Move_Left ) ){
             rotVec = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( mBinds.isBindDown( UserBinds::Action::Player_Move_Right ) ){
             rotVec = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( mBinds.isBindDown( UserBinds::Action::Player_Move_Forward ) ){
             rotVec = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }
        
        if( mBinds.isBindDown( UserBinds::Action::Player_Move_Backward ) ){
             rotVec = XMVectorSet( -1.0f, 0.0f, 0.0f, 0.0f );
             rotVec = XMVector4Transform( rotVec, rotMat );
             moveVec += rotVec;
        }

        //TEMPORARY WAY OF SOLVING THIS!
        //As long as we are not on a ramp, bring us to the floor
        int bx = static_cast<int>( mEntity.getPosition().x / 0.3f);
        int bz = static_cast<int>( mEntity.getPosition().z / 0.3f);

        CLAMP( bx, 0, mWorld.getLevel().getWidth() - 1 );
        CLAMP( bz, 0, mWorld.getLevel().getDepth() - 1 );

        if( mWorld.getLevel().getBlockRamp(bx, bz) == Level::Ramp::None ){
            //Make sure we are on the floor, otherwise bring us down through gravity
            float distFromGround = ( mEntity.getPosition().y - mEntity.getSolidity().height ) - static_cast<float>(mWorld.getLevel().getBlockHeight(bx, bz)) * 0.3f;

            if( distFromGround > 0.05f ){
                moveVec -= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            }else if(distFromGround < -0.05f ){
                moveVec += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            }
        }else if( mWorld.getLevel().getBlockRamp(bx, bz) == Level::Ramp::Front ){
            float blockHeight = static_cast<float>(mWorld.getLevel().getBlockHeight(bx, bz)) * 0.3f;
            float rampUp = fmod(mEntity.getPosition().z, 0.3f);
            rampUp *= 1.25f;
            mEntity.getPosition().y = blockHeight + rampUp + mEntity.getSolidity().height;
        }else if( mWorld.getLevel().getBlockRamp(bx, bz) == Level::Ramp::Back ){
            float blockHeight = static_cast<float>(mWorld.getLevel().getBlockHeight(bx, bz)) * 0.3f;
            float rampUp = fmod(mEntity.getPosition().z, 0.3f);
            rampUp = (0.3f - rampUp) * 1.25f;
            mEntity.getPosition().y = blockHeight + rampUp + mEntity.getSolidity().height;
        }else if( mWorld.getLevel().getBlockRamp(bx, bz) == Level::Ramp::Left ){
            float blockHeight = static_cast<float>(mWorld.getLevel().getBlockHeight(bx, bz)) * 0.3f;
            float rampUp = fmod(mEntity.getPosition().x, 0.3f);
            rampUp *= 1.25f;
            mEntity.getPosition().y = blockHeight + rampUp + mEntity.getSolidity().height;
        }else if( mWorld.getLevel().getBlockRamp(bx, bz) == Level::Ramp::Right ){
            float blockHeight = static_cast<float>(mWorld.getLevel().getBlockHeight(bx, bz)) * 0.3f;
            float rampUp = fmod(mEntity.getPosition().x, 0.3f);
            rampUp = (0.3f - rampUp) * 1.25f;
            mEntity.getPosition().y = blockHeight + rampUp + mEntity.getSolidity().height;
        }

        moveVec = XMVector3Normalize( moveVec );
        moveVec = XMVectorSetW( moveVec, 1.0f );

        mWorld.moveEntity( &mEntity, moveVec, 2.0f * dt );

        mCamera.getPosition() = mEntity.getPosition();
    }else{
        if( camKeyDown[0] ){
            mCamera.moveForwardBack( 2.0f * dt ); 
        }
        
        if( camKeyDown[1] ){
            mCamera.moveForwardBack( -2.0f * dt ); 
        }
        
        if( camKeyDown[2] ){
            mCamera.moveLeftRight( 2.0f * dt ); 
        }
        
        if( camKeyDown[3] ){
            mCamera.moveLeftRight( -2.0f * dt ); 
        }
    }

    //Get the Cursor position on the window 
    POINT p;
    GetCursorPos( &p );
    ScreenToClient(mWindow.getWindowHandle(), &p);

    //Convert it to -1 to 1 
    XMFLOAT2 mousePos( 
        static_cast<float>(p.x) / static_cast<float>(mWindow.getWindowWidth()), 
        static_cast<float>(p.y) / static_cast<float>(mWindow.getWindowHeight()) );

    mousePos.x *= 2.0f; mousePos.x -= 1.0f;
    mousePos.y *= 2.0f; mousePos.y -= 1.0f;

    //Print coords
    sprintf(MousePosString, "Mouse: %.2f, %.2f", mousePos.x, mousePos.y);

    //Don't update the UI if we aren't drawing it... duh
    if( drawUI ){
        UIWindow::UserChange change = mUIWindow.update( mLeftClick, mousePos, mKeyPress, (byte)mKeyChar );

        //Reset key press if it is true so we don't spam it
        if( mKeyPress ){
            mKeyPress = false;
        }

        if( change.action == UIWindow::UserChange::Action::ClickButton &&
            change.id == 0 ){
            mWorldGen.genLevel( mWorld.getLevel(), mLevelPreset );
            mWorldDisplay.getLevelDisplay().createMeshFromLevel( mWindow.getDevice(), mWorld.getLevel(), 0.3f, 0.3f);
        }else if( change.action == UIWindow::UserChange::Action::MoveSlider &&
            change.id == 0 ){
            mCamera.setFOV( 40.0f + ( 20.0f * mSlider->getPercent() ) );
        }else if(change.action == UIWindow::UserChange::Action::SelectDropOption && change.id == 0 ){
            switch( mDropBox->getSelectedOption() ){
            case 0:
                mWorldDisplay.getLevelDisplay().setFog( XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), 15.0f );
                break;
            case 1:
                mWorldDisplay.getLevelDisplay().setFog( XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), 15.0f );
                break;
            case 2:
                mWorldDisplay.getLevelDisplay().setFog( XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), 15.0f );
                break;
            default:
                break;
            }
        }
    }
}

void App::draw( )
{
    //Clear Back Buffer, Depth Buffer and Stencil buffer
    mWindow.clearBDS();

	//save the previous render target and depth stencil
	ID3D11RenderTargetView* prevRTV;
	ID3D11DepthStencilView* prevDSV;

	mWindow.getDeviceContext()->OMGetRenderTargets(1, &prevRTV, &prevDSV);

	//set gbuffer render targets
    mWindow.getDeviceContext()->OMSetRenderTargets(2, mGBufferRTVs, mDepthStencilView);

	//clear gbuffer render targets
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float clearNormal[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

	mWindow.getDeviceContext()->ClearRenderTargetView(mGBufferRTVs[0], clearColor);
	mWindow.getDeviceContext()->ClearRenderTargetView(mGBufferRTVs[1], clearNormal);
    mWindow.getDeviceContext()->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set constants
    XMMATRIX view  = XMLoadFloat4x4(&mCamera.getView());
    XMMATRIX proj  = XMLoadFloat4x4(&mCamera.getProj());
	XMMATRIX viewProj = view * proj;
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
	XMMATRIX transVP = XMMatrixTranspose(viewProj);
    XMMATRIX transIVP = XMMatrixTranspose(invViewProj);

    XMMATRIX identity = XMMatrixIdentity();

    //Set the View Projection Matrix and it's inverse
	ID3DX11EffectMatrixVariable* mfxViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mfxViewProj->SetMatrix(reinterpret_cast<float*>(&viewProj));
    
    ID3DX11EffectMatrixVariable* mfxInvViewProj = mFX->GetVariableByName("gInvViewProj")->AsMatrix();
    mfxInvViewProj->SetMatrix(reinterpret_cast<float*>(&invViewProj));

	//Start geometry fill pass
	D3DX11_TECHNIQUE_DESC techDesc;
	mRenderGBufferTech->GetDesc( &techDesc );

	for(ushort p = 0; p < techDesc.Passes; ++p)
	{
		mRenderGBufferTech->GetPassByIndex(p)->Apply(0, mWindow.getDeviceContext());
        mWorldDisplay.draw( mWindow.getDeviceContext(), mFX, mWorld );
		//drawDarkParticles
	}

	//switch back to original render target
	mWindow.getDeviceContext()->OMSetRenderTargets(1, &prevRTV, prevDSV);	

	//pass in GBuffer to the lighting shaders
	ID3DX11EffectShaderResourceVariable* mfxcolorbuf = mFX->GetVariableByName("colorBuffer")->AsShaderResource();
	mfxcolorbuf->SetResource(mGBufferSRVs[0]);

	ID3DX11EffectShaderResourceVariable* mfxnormalbuf = mFX->GetVariableByName("normalBuffer")->AsShaderResource();
	mfxnormalbuf->SetResource(mGBufferSRVs[1]);

	ID3DX11EffectShaderResourceVariable* mfxdepthbuf = mFX->GetVariableByName("depthBuffer")->AsShaderResource();
	mfxdepthbuf->SetResource(mGBufferSRVs[2]);

	ID3D11DepthStencilState* prevDSS;
	mWindow.getDeviceContext()->OMGetDepthStencilState(&prevDSS, 0);

	ID3D11BlendState* prevBS;
	FLOAT prevfloat[4];
	UINT prevMask = 0;

	mWindow.getDeviceContext()->OMGetBlendState(&prevBS, prevfloat, &prevMask);
	   
	//start lighting pass
	mAmbientLightTech->GetDesc( &techDesc );
    for(ushort p = 0; p < techDesc.Passes; ++p)
	{
		mAmbientLightTech->GetPassByIndex(p)->Apply(0, mWindow.getDeviceContext());	
		mWorldDisplay.getLevelDisplay().applyFog(mWindow.getDeviceContext()); 
		
		//draw fullscreen quad to spawn the lighting post process
		drawFSQuad();
	}

	mPointLightTech->GetDesc( &techDesc );
    for(ushort p = 0; p < techDesc.Passes; ++p)
	{
		mPointLightTech->GetPassByIndex(p)->Apply(0, mWindow.getDeviceContext());
		mWorldDisplay.getLevelDisplay().applyFog(mWindow.getDeviceContext()); 
		mWorldDisplay.drawPointLights(mWindow.getDeviceContext(), mFX, mCamera.getPosition(), mWorld);
	}

	//pass for LightParticles


	mWindow.getDeviceContext()->OMSetDepthStencilState(prevDSS, 0);

	mWindow.getDeviceContext()->OMSetBlendState(prevBS, prevfloat, prevMask);
    
    mWindow.getDeviceContext()->RSSetState( NULL );
    
    //For testing drawing windows
    if( drawUI ){
        mUIDisplay.buildWindowVB( mUIWindow, mWindow.getAspectRatio() );
    }

    //Draw!
    mUIDisplay.drawUI( mWindow.getDeviceContext() );

    if( drawUI ){
        mUIDisplay.drawWindowText( mWindow.getDeviceContext(), mUIWindow, mTextManager );
    }

    if( mCheckBox->isChecked() ){
        mTextManager.drawString(mWindow.getDeviceContext(), FPSString, -1.0f, -1.0f);
        mTextManager.drawString(mWindow.getDeviceContext(), CameraPosString, -1.0f, -0.95f);
        mTextManager.drawString(mWindow.getDeviceContext(), MousePosString, -1.0f, -0.9f);
    
        char buffer[128];

        sprintf(buffer, "Toggle UI Drawing: U : %s", drawUI ? "On" : "Off");
        mTextManager.drawString(mWindow.getDeviceContext(), buffer, -1.0f, 0.8f);

        sprintf(buffer, "Toggle Free Look: I : %s", freeLook ? "On" : "Off");
        mTextManager.drawString(mWindow.getDeviceContext(), buffer, -1.0f, 0.85f);

        sprintf(buffer, "Toggle Collision: O : %s", collisionMode ? "On" : "Off");
        mTextManager.drawString(mWindow.getDeviceContext(), buffer, -1.0f, 0.9f);

        sprintf(buffer, "Generate New Dungeon: R");
        mTextManager.drawString(mWindow.getDeviceContext(), buffer, -1.0f, 0.95f);

    }

    mWindow.getSwapChain()->Present(0, 0);
}

void App::drawFSQuad( )
{
	UINT stride = sizeof(EnvVertex);
    UINT offset = 0;

	mWindow.getDeviceContext()->IASetIndexBuffer( mFSQuadIB, DXGI_FORMAT_R16_UINT, 0 );
	mWindow.getDeviceContext()->IASetVertexBuffers(0, 1, &mFSQuadVB, &stride, &offset);

    mWindow.getDeviceContext()->DrawIndexed(6, 0, 0);
}

void App::clear( )
{
    mWorldDisplay.clear();
    mTextManager.clear();

    mUIWindow.clear();

    mUIDisplay.clear();

    mWindow.clear();
}