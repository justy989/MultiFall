#include "LevelDisplay.h"
#include "Log.h"

LevelDisplay::LevelDisplay() :
    mInputLayout(NULL),
    mWorldCB(NULL),
    mFloorVB(NULL),
    mFloorIB(NULL),
    mWallsVB(NULL),
    mWallsIB(NULL),
    mRampWallsVB(NULL),
    mFloorTexture(NULL),
    mWallTexture(NULL),
    mBlockCount(0),
    mWallCount(0),
    mRampCount(0),
    mFloorClip(1.0f),
    mWallClip(1.0f),
    mFloorTileRows(4)
{

}

LevelDisplay::~LevelDisplay()
{
	clear();
}

bool LevelDisplay::init( ID3D11Device* device, ID3DX11EffectTechnique* technique )
{
    clear();

    // Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//Create the input layout
    D3DX11_PASS_DESC passDesc;
    technique->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Vertex Input Layout for Level" << LOG_ENDL;
            return false;
    }    

    D3D11_BUFFER_DESC constDesc;
    ZeroMemory( &constDesc, sizeof( constDesc ) );
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof( XMMATRIX );
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mWorldCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for Level" << LOG_ENDL;
        return false;
    }

	if( !mTorch.loadFromObj(device, "content/meshes/torch.obj", L"content/textures/torch_texture.png") ){
        return false;
    }

    LOG_INFO << "Created Input Description and Texture Sampler for Level" << LOG_ENDL;
    return true;
}

bool LevelDisplay::setTextures( ID3D11Device* device, LPCWSTR floorTexturePath, float floorClip, LPCWSTR wallTexturePath, float wallClip )
{
    HRESULT hr;

    //Release the current textures first
    ReleaseCOM( mFloorTexture );
    ReleaseCOM( mWallTexture );

    //Try to create the ones passed in
    hr = D3DX11CreateShaderResourceViewFromFile( device,
        floorTexturePath, 0, 0, &mFloorTexture, 0 );

    if( FAILED(hr) ){
        LOG_ERRO << "Unable to load floor texture: " << LOG_WC_TO_C(floorTexturePath) << LOG_ENDL;
        return false;
    }

    hr = D3DX11CreateShaderResourceViewFromFile( device,
        wallTexturePath, 0, 0, &mWallTexture, 0 );

    if( FAILED(hr) ){
        LOG_ERRO << "Unable to load wall texture: " << LOG_WC_TO_C(wallTexturePath) << LOG_ENDL;
        return false;
    }

    mFloorClip = floorClip;
    mWallClip = wallClip;

    mFloorTileRows = static_cast<int>( 1.0f / floorClip );

    LOG_INFO << "Loaded Level Floor Texture: " << LOG_WC_TO_C(floorTexturePath) << LOG_ENDL;
    LOG_INFO << "Loaded Level Wall Texture: " << LOG_WC_TO_C(wallTexturePath) << LOG_ENDL;
    return true;
}

void LevelDisplay::clear()
{
    ReleaseCOM( mInputLayout );
    ReleaseCOM( mWorldCB );
    ReleaseCOM( mFloorVB );
    ReleaseCOM( mFloorIB );
    ReleaseCOM( mWallsVB );
    ReleaseCOM( mWallsIB );
    ReleaseCOM( mRampWallsVB );
    ReleaseCOM( mFloorTexture );
    ReleaseCOM( mWallTexture );

    LOG_INFO << "Released Level Display Assets and buffers" << LOG_ENDL;
}

bool LevelDisplay::createMeshFromLevel( ID3D11Device* device, Level& level, float blockDimension, float heightInterval )
{
    //Create the different parts of the level separately so they can be textured separately

    if( !createFloorMesh( device, level, blockDimension, heightInterval ) ){
        return false;
    }

    if( !createWallsMesh( device, level, blockDimension, heightInterval ) ){
        return false;
    }

    LOG_INFO << "Level Mesh has been generated" << LOG_ENDL;
    return true;
}

void LevelDisplay::setFog( XMFLOAT4& fogColor, float fogStart, float fogEnd )
{
    mFog.color = fogColor;
    mFog.start = fogStart;
    mFog.end = fogEnd;

    //Update constant buffer
}

void LevelDisplay::draw( ID3D11DeviceContext* device, ID3DX11Effect* fx, World& world )
{
    UINT stride = sizeof(DungeonVertex);
    UINT offset = 0;

    //Update the world matrix
    XMMATRIX worldm = XMMatrixIdentity();
    //ID3DX11EffectMatrixVariable* mFXWorld = fx->GetVariableByName("gWorld")->AsMatrix();
	//mFXWorld->SetMatrix(reinterpret_cast<float*>(&world));

    //Set input layout and topology
    device->IASetInputLayout( mInputLayout );
    device->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    //Update the world matrix
    device->UpdateSubresource( mWorldCB, 0, 0, &worldm, 0, 0 ); 
    device->VSSetConstantBuffers( 1, 1, &mWorldCB );

    //Set the floor texture
    device->PSSetShaderResources(0, 1, &mFloorTexture );

    //Draw the floor and ramps
    device->IASetIndexBuffer( mFloorIB, DXGI_FORMAT_R16_UINT, 0 );
    device->IASetVertexBuffers(0, 1, &mFloorVB, &stride, &offset);
    device->DrawIndexed(6 * mBlockCount, 0, 0);

    //Set the wall texture
    device->PSSetShaderResources(0, 1, &mWallTexture );

    //Draw ramp walls separately
    device->IASetVertexBuffers(0, 1, &mRampWallsVB, &stride, &offset);
    device->Draw(6 * mRampCount, 0);

    //Draw the walls
    device->IASetIndexBuffer( mWallsIB, DXGI_FORMAT_R16_UINT, 0 );
    device->IASetVertexBuffers(0, 1, &mWallsVB, &stride, &offset);
    device->DrawIndexed(6 * mWallCount, 0, 0);

	Level& level = world.getLevel();
	for(int i = 0; i < level.getNumTorches(); i++)
	{
		worldm = level.getTorch(i).world;

		worldm = XMMatrixTranspose( worldm );

		device->UpdateSubresource( mWorldCB, 0, 0, &worldm, 0, 0 ); 
		device->VSSetConstantBuffers( 1, 1, &mWorldCB );

		if(level.getTorch(i).mesh == 0)
		{
			mTorch.draw(device);
		}
		
	}

}

bool LevelDisplay::createFloorMesh( ID3D11Device* device, Level& level, float blockDimension, float heightInterval  )
{
    ReleaseCOM( mFloorVB );
    ReleaseCOM( mFloorIB );

    mBlockCount = level.getWidth() * level.getDepth();

    DungeonVertex* verts = new DungeonVertex[ 4 * mBlockCount ];
    ushort* inds = new ushort[ 6 * mBlockCount ];

    int v = 0;

    //Generate floor blocks based on height
    for(int i = 0; i < level.getWidth(); i++){
        for(int j = 0; j < level.getDepth(); j++){

            //Don't generate a floor that is a ramp or the same height as the ceiling, no one will see it silly!
            if( level.getBlockHeight(i, j) == level.getHeight() ){
                continue;
            }

			byte id = level.getBlockTileID(i,j);
            float row = id / mFloorTileRows;
			float column = id % mFloorTileRows;

            //Front left
            verts[ v ].position.x = static_cast<float>(i) * blockDimension;
            verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
            verts[ v ].position.z = static_cast<float>(j) * blockDimension;
			verts[ v ].tex = XMFLOAT2(column * mFloorClip, row * mFloorClip);

            v++;

            //Front right
            verts[ v ].position.x = verts[ v - 1 ].position.x + blockDimension;
            verts[ v ].position.y = verts[ v - 1 ].position.y;
            verts[ v ].position.z = verts[ v - 1 ].position.z;
			verts[ v ].tex = XMFLOAT2(column * mFloorClip, row * mFloorClip + mFloorClip);

            v++;

            //Back left
            verts[ v ].position.x = verts[ v - 2 ].position.x;
            verts[ v ].position.y = verts[ v - 2 ].position.y;
            verts[ v ].position.z = verts[ v - 2 ].position.z + blockDimension;
			verts[ v ].tex = XMFLOAT2(column * mFloorClip + mFloorClip, row * mFloorClip);

            v++;

            //Back right
            verts[ v ].position.x = verts[ v - 3 ].position.x + blockDimension;
            verts[ v ].position.y = verts[ v - 3 ].position.y;
            verts[ v ].position.z = verts[ v - 3 ].position.z + blockDimension;
			verts[ v ].tex = XMFLOAT2(column * mFloorClip + mFloorClip, row * mFloorClip + mFloorClip);

            v++;

            //Raise the verticies of the correct side of the ramp
            switch( level.getBlockRamp(i, j) ){
            case Level::Ramp::Front:
                verts[ v - 1 ].position.y += heightInterval;
                verts[ v - 2 ].position.y += heightInterval;
                break;
            case Level::Ramp::Back:
                verts[ v - 3 ].position.y += heightInterval;
                verts[ v - 4 ].position.y += heightInterval;
                break;
            case Level::Ramp::Left:
                verts[ v - 1 ].position.y += heightInterval;
                verts[ v - 3 ].position.y += heightInterval;
                break;
            case Level::Ramp::Right:
                verts[ v - 2 ].position.y += heightInterval;
                verts[ v - 4 ].position.y += heightInterval;
                break;
            default:
                break;
            }

            //Generate the normals
			DungeonVertex::createSurfaceNormals(verts[v-4], verts[v-2], verts[v-3]);
			DungeonVertex::createSurfaceNormals(verts[v-3], verts[v-2], verts[v-1]);
        }
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(DungeonVertex) * (mBlockCount * 4);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = verts;

    if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mFloorVB))){
        LOG_ERRO << "Unable to allocate Vertex buffer for level floors" << LOG_INFO;
        return false;
    }

    //Reusing tmp variable for indexing index buffer!
    v = 0;

    int index = 0;

    //Generate indices corresponding to generated verts
    for(int i = 0; i < level.getWidth(); i++){
        for(int j = 0; j < level.getDepth(); j++){
            inds[v] = index;
            inds[v+1] = index + 2;
            inds[v+2] = index + 1;

            inds[v+3] = index + 1;
            inds[v+4] = index + 2;
            inds[v+5] = index + 3;

            v += 6;
            index += 4;
        }
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(ushort) * mBlockCount * 6;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = inds;

    //Create VB
    if(FAILED(device->CreateBuffer(&ibd, &iinitData, &mFloorIB))){
        LOG_ERRO << "Unable to allocate index buffer for level floors" << LOG_INFO;
        return false;
    }

    delete[] verts;
    delete[] inds;

    return true;
}

bool LevelDisplay::createWallsMesh( ID3D11Device* device, Level& level, float blockDimension, float heightInterval  )
{
    ReleaseCOM( mWallsIB );
    ReleaseCOM( mWallsVB );

    ReleaseCOM( mRampWallsVB );

    int v = 0;
    int indexCount = 0;
    int vertexCount = 0;

    DungeonVertex* verts = new DungeonVertex[ 4 * 4 * mBlockCount ];

    mWallCount = 0;

	//mLightManager.addPointLight(XMFLOAT3(0,0,0), 1.0f);

    for(int i = 0; i < level.getWidth(); i++){
        for(int j = 0; j < level.getDepth(); j++){ 

            //Make Left Wall
            int nextI = i - 1;
            int nextJ = j;
			
			
            if( nextI >= 0 ){
                if( level.getBlockHeight( i, j ) > level.getBlockHeight( nextI, nextJ ) ){

                    verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                    verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval ;
                    verts[ v ].position.z = static_cast<float>(j) * blockDimension;
					verts[ v ].tex = XMFLOAT2(1, 0);

					if(i % 3 == 0 && j % 4 == 0 && (level.getBlockHeight( i, j ) - level.getBlockHeight( nextI, nextJ )) > 1)
					{
                        PointLight pl;
						XMFLOAT3 pos = verts[v].position;
						pos.y -= 0.5f * blockDimension;
						pos.z += 0.5f * blockDimension;
                        pl.set( pos, pl.getRadius(), pl.getIntensity(), pl.getColor() );
                        level.addLight( pl );

						level.addTorch(pos, XMConvertToRadians(-90.0f));
					}
					
                    v++;

                    verts[ v ].position.x = verts[ v - 1 ].position.x;
                    verts[ v ].position.y = verts[ v - 1 ].position.y;
                    verts[ v ].position.z = verts[ v - 1 ].position.z + blockDimension;
					verts[ v ].tex = XMFLOAT2(0, 0);
					
                    v++;

                    verts[ v ].position.x = verts[ v - 2 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 2 ].position.z;
					verts[ v ].tex = XMFLOAT2(1, 1*level.getBlockHeight( i, j ));

                    v++;

                    verts[ v ].position.x = verts[ v - 3 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 3 ].position.z + blockDimension;
					verts[ v ].tex = XMFLOAT2(0, 1*level.getBlockHeight( i, j ));

                    v++;

                    vertexCount += 4;
                    indexCount += 6;
                    mWallCount++;

					DungeonVertex::createSurfaceNormals(verts[v-4], verts[v-2], verts[v-3]);
					DungeonVertex::createSurfaceNormals(verts[v-3], verts[v-2], verts[v-1]);
                }
            }

            //Make Right wall
            nextI = i + 1;
            nextJ = j;

            if( nextI < level.getWidth() ){
                if( level.getBlockHeight( i, j ) > level.getBlockHeight( nextI, nextJ ) ){
                    verts[ v ].position.x = static_cast<float>(i) * blockDimension + blockDimension;
                    verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                    verts[ v ].position.z = static_cast<float>(j) * blockDimension + blockDimension;
					verts[ v ].tex = XMFLOAT2(1, 0);

                    v++;
                    
                    verts[ v ].position.x = verts[ v - 1 ].position.x;
                    verts[ v ].position.y = verts[ v - 1 ].position.y;
                    verts[ v ].position.z = verts[ v - 1 ].position.z - blockDimension;
					verts[ v ].tex = XMFLOAT2(0,0);

                    v++;

                    verts[ v ].position.x = verts[ v - 2 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 2 ].position.z;
					verts[ v ].tex = XMFLOAT2(1, 1*level.getBlockHeight( i, j ));

                    v++;

                    verts[ v ].position.x = verts[ v - 3 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 3 ].position.z - blockDimension;
					verts[ v ].tex = XMFLOAT2(0, 1*level.getBlockHeight( i, j ));

                    v++;

                    vertexCount += 4;
                    indexCount += 6;
                    mWallCount++;

					DungeonVertex::createSurfaceNormals(verts[v-4], verts[v-2], verts[v-3]);
					DungeonVertex::createSurfaceNormals(verts[v-3], verts[v-2], verts[v-1]);
                }
            }

            //Make Front Wall
            nextI = i;
            nextJ = j - 1;

            if( nextJ >= 0 ){
                if( level.getBlockHeight( i, j ) > level.getBlockHeight( nextI, nextJ ) ){
                    verts[ v ].position.x = static_cast<float>(i) * blockDimension + blockDimension;
                    verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                    verts[ v ].position.z = static_cast<float>(j) * blockDimension;
					verts[ v ].tex = XMFLOAT2(1, 0);

                    v++;
                    
                    verts[ v ].position.x = verts[ v - 1 ].position.x - blockDimension;
                    verts[ v ].position.y = verts[ v - 1 ].position.y;
                    verts[ v ].position.z = verts[ v - 1 ].position.z;
					verts[ v ].tex = XMFLOAT2(0,0);

                    v++;

                    verts[ v ].position.x = verts[ v - 2 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 2 ].position.z;
					verts[ v ].tex = XMFLOAT2(1, 1*level.getBlockHeight( i, j ));

                    v++;

                    verts[ v ].position.x = verts[ v - 3 ].position.x - blockDimension;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 3 ].position.z;
					verts[ v ].tex = XMFLOAT2(0, 1*level.getBlockHeight( i, j ));

                    v++;

                    vertexCount += 4;
                    indexCount += 6;
                    mWallCount++;

					DungeonVertex::createSurfaceNormals(verts[v-4], verts[v-2], verts[v-3]);
					DungeonVertex::createSurfaceNormals(verts[v-3], verts[v-2], verts[v-1]);
                }
            }

            //Make back wall
            nextI = i;
            nextJ = j + 1;

            if( nextJ < level.getDepth() ){
                if( level.getBlockHeight( i, j ) > level.getBlockHeight( nextI, nextJ ) ){
                    verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                    verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                    verts[ v ].position.z = static_cast<float>(j) * blockDimension + blockDimension;
					verts[ v ].tex = XMFLOAT2(1, 0);

                    v++;
                    
                    verts[ v ].position.x = verts[ v - 1 ].position.x + blockDimension;
                    verts[ v ].position.y = verts[ v - 1 ].position.y;
                    verts[ v ].position.z = verts[ v - 1 ].position.z;
					verts[ v ].tex = XMFLOAT2(0,0);

                    v++;

                    verts[ v ].position.x = verts[ v - 2 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 2 ].position.z;
					verts[ v ].tex = XMFLOAT2(1, 1*level.getBlockHeight( i, j ));

                    v++;

                    verts[ v ].position.x = verts[ v - 3 ].position.x + blockDimension;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 3 ].position.z;
					verts[ v ].tex = XMFLOAT2(0, 1*level.getBlockHeight( i, j ));

                    v++;

                    vertexCount += 4;
                    indexCount += 6;
                    mWallCount++;

					DungeonVertex::createSurfaceNormals(verts[v-4], verts[v-2], verts[v-3]);
					DungeonVertex::createSurfaceNormals(verts[v-3], verts[v-2], verts[v-1]);
                }
            }
        }
    }

    if( vertexCount ){
        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = sizeof(DungeonVertex) * vertexCount;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;
	    vbd.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = verts;

        if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mWallsVB))){
            LOG_ERRO << "Unable to allocate Vertex buffer for level walls" << LOG_INFO;
            return false;
        }

        delete[] verts;
    }

    if( indexCount ){
        ushort* inds = new ushort[ indexCount ];

        v = 0;
        int index = 0;

        for(; v < indexCount;){
            inds[v] = index;
            inds[v+1] = index + 2;
            inds[v+2] = index + 1;

            inds[v+3] = index + 1;
            inds[v+4] = index + 2;
            inds[v+5] = index + 3;

            v += 6;
            index += 4;
        }

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof(ushort) * indexCount;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;
	    ibd.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA iinitData;
        iinitData.pSysMem = inds;

        if(FAILED(device->CreateBuffer(&ibd, &iinitData, &mWallsIB))){
            LOG_ERRO << "Unable to allocate index buffer for level walls" << LOG_INFO;
            return false;
        }
        
        delete[] inds;
    }

    mRampCount = 0;
    indexCount = 0;
    vertexCount = 0;

    verts = new DungeonVertex[ mBlockCount * 6 ];

    v = 0;

    for(int i = 0; i < level.getWidth(); i++){
        for(int j = 0; j < level.getDepth(); j++){

            if( level.getBlockRamp(i,j) == Level::Ramp::Front ){
                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;
                verts[ v ].tex = XMFLOAT2(0, 0);

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y;
                verts[ v ].position.z = verts[ v - 1 ].position.z + blockDimension;
                verts[ v ].tex = XMFLOAT2(1, 0);

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y + heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z + blockDimension;
                verts[ v ].tex = XMFLOAT2(1, 1);

                v++;

                verts[ v ].position.x = static_cast<float>(i) * blockDimension + blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;
                verts[ v ].tex = XMFLOAT2(0, 0);

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y + heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z + blockDimension;
                verts[ v ].tex = XMFLOAT2(1, 1);

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y;
                verts[ v ].position.z = verts[ v - 2 ].position.z + blockDimension;
                verts[ v ].tex = XMFLOAT2(1, 0);

                v++;

                DungeonVertex::createSurfaceNormals( verts[v - 6], verts[v - 5], verts[v - 4] ); 
                DungeonVertex::createSurfaceNormals( verts[v - 3], verts[v - 2], verts[v - 1] ); 

                vertexCount += 6;
                mRampCount++;
            }else if( level.getBlockRamp(i,j) == Level::Ramp::Back ){
                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval + heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;
                verts[ v ].tex = XMFLOAT2(0, 1);

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z;
                verts[ v ].tex = XMFLOAT2(0, 0);

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z + blockDimension;
                verts[ v ].tex = XMFLOAT2(1, 0);

                v++;

                verts[ v ].position.x = static_cast<float>(i) * blockDimension + blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval + heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;
                verts[ v ].tex = XMFLOAT2(0, 1);

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z + blockDimension;
                verts[ v ].tex = XMFLOAT2(1, 0);

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z;
                verts[ v ].tex = XMFLOAT2(0, 0);

                v++;

                DungeonVertex::createSurfaceNormals( verts[v - 6], verts[v - 5], verts[v - 4] ); 
                DungeonVertex::createSurfaceNormals( verts[v - 3], verts[v - 2], verts[v - 1] ); 

                vertexCount += 6;
                mRampCount++;
            }else if( level.getBlockRamp(i,j) == Level::Ramp::Left ){
                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;
                verts[ v ].tex = XMFLOAT2(0, 0);

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 1 ].position.y + heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z;
                verts[ v ].tex = XMFLOAT2(1, 1);

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 2 ].position.y;
                verts[ v ].position.z = verts[ v - 2 ].position.z;
                verts[ v ].tex = XMFLOAT2(1, 0);

                v++;

                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension + blockDimension;
                verts[ v ].tex = XMFLOAT2(0, 0);

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 1 ].position.y;
                verts[ v ].position.z = verts[ v - 1 ].position.z;
                verts[ v ].tex = XMFLOAT2(1, 0);

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 2 ].position.y + heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z;
                verts[ v ].tex = XMFLOAT2(1, 1);

                v++;

                DungeonVertex::createSurfaceNormals( verts[v - 6], verts[v - 5], verts[v - 4] ); 
                DungeonVertex::createSurfaceNormals( verts[v - 3], verts[v - 2], verts[v - 1] ); 

                vertexCount += 6;
                mRampCount++;
            }else if( level.getBlockRamp(i,j) == Level::Ramp::Right ){
                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval + heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension + blockDimension;
                verts[ v ].tex = XMFLOAT2(0, 1);

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z;
                verts[ v ].tex = XMFLOAT2(0, 0);

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 2 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z;
                verts[ v ].tex = XMFLOAT2(1, 0);

                v++;

                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval + heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;
                verts[ v ].tex = XMFLOAT2(0, 1);

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 1 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z;
                verts[ v ].tex = XMFLOAT2(1, 0);

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z;
                verts[ v ].tex = XMFLOAT2(0, 0);

                v++;

                DungeonVertex::createSurfaceNormals( verts[v - 6], verts[v - 5], verts[v - 4] ); 
                DungeonVertex::createSurfaceNormals( verts[v - 3], verts[v - 2], verts[v - 1] ); 

                vertexCount += 6;
                mRampCount++;
            }
        }
    }

    if( vertexCount ){
        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = sizeof(DungeonVertex) * vertexCount;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;
	    vbd.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = verts;

        if( mRampCount ){
            if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mRampWallsVB))){
                LOG_ERRO << "Unable to allocate Vertex buffer for ramp walls" << LOG_INFO;
                return false;
            }
        }
    }
    
    delete[] verts;

    return true;
}
