#include "LevelDisplay.h"
#include "Log.h"

LevelDisplay::LevelDisplay() :
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
    mWallClip(1.0f)
{

}

LevelDisplay::~LevelDisplay()
{
    clear();
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

    LOG_INFO << "Loaded Level Floor Texture: " << LOG_WC_TO_C(floorTexturePath) << LOG_ENDL;
    LOG_INFO << "Loaded Level Wall Texture: " << LOG_WC_TO_C(wallTexturePath) << LOG_ENDL;
    return true;
}

void LevelDisplay::clear()
{
    ReleaseCOM( mFloorVB );
    ReleaseCOM( mFloorIB );
    ReleaseCOM( mWallsVB );
    ReleaseCOM( mWallsIB );
    ReleaseCOM( mRampWallsVB );
    ReleaseCOM( mFloorTexture );
    ReleaseCOM( mWallTexture );
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

void LevelDisplay::draw( ID3D11DeviceContext* device, ID3DX11EffectTechnique* tech, ID3D11SamplerState* textureSampler )
{
    UINT stride = sizeof(DungeonVertex);
    UINT offset = 0;

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );

    for(ushort p = 0; p < techDesc.Passes; ++p){
        tech->GetPassByIndex(p)->Apply(0, device);

        //Draw the floor and ramps
        device->IASetIndexBuffer( mFloorIB, DXGI_FORMAT_R16_UINT, 0 );
        device->IASetVertexBuffers(0, 1, &mFloorVB, &stride, &offset);
        
        device->PSSetSamplers(0, 1, &textureSampler );
		device->PSSetShaderResources( 0, 1, &mFloorTexture );

        device->DrawIndexed(6 * mBlockCount, 0, 0);

        //Draw ramp walls separately
        device->IASetVertexBuffers(0, 1, &mRampWallsVB, &stride, &offset);
        device->Draw(mRampCount * 6, 0);

        //Draw the walls
        device->IASetIndexBuffer( mWallsIB, DXGI_FORMAT_R16_UINT, 0 );
        device->IASetVertexBuffers(0, 1, &mWallsVB, &stride, &offset);

		device->PSSetShaderResources( 0, 1, &mWallTexture );

        device->DrawIndexed(6 *mWallCount, 0, 0);
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
			float row = id / 4;
			float column = id % 4;

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
            case Level::Block::Ramp::Front:
                verts[ v - 1 ].position.y += heightInterval;
                verts[ v - 2 ].position.y += heightInterval;
                break;
            case Level::Block::Ramp::Back:
                verts[ v - 3 ].position.y += heightInterval;
                verts[ v - 4 ].position.y += heightInterval;
                break;
            case Level::Block::Ramp::Left:
                verts[ v - 1 ].position.y += heightInterval;
                verts[ v - 3 ].position.y += heightInterval;
                break;
            case Level::Block::Ramp::Right:
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

            if( level.getBlockRamp(i,j) == Level::Block::Ramp::Front ){
                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y;
                verts[ v ].position.z = verts[ v - 1 ].position.z + blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y + heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z + blockDimension;

                v++;

                verts[ v ].position.x = static_cast<float>(i) * blockDimension + blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y + heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z + blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y;
                verts[ v ].position.z = verts[ v - 2 ].position.z + blockDimension;

                v++;

                DungeonVertex::createSurfaceNormals( verts[v - 6], verts[v - 5], verts[v - 4] ); 
                DungeonVertex::createSurfaceNormals( verts[v - 3], verts[v - 2], verts[v - 1] ); 

                vertexCount += 6;
                mRampCount++;
            }else if( level.getBlockRamp(i,j) == Level::Block::Ramp::Back ){
                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval + heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z;

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z + blockDimension;

                v++;

                verts[ v ].position.x = static_cast<float>(i) * blockDimension + blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval + heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z + blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z;

                v++;

                DungeonVertex::createSurfaceNormals( verts[v - 6], verts[v - 5], verts[v - 4] ); 
                DungeonVertex::createSurfaceNormals( verts[v - 3], verts[v - 2], verts[v - 1] ); 

                vertexCount += 6;
                mRampCount++;
            }else if( level.getBlockRamp(i,j) == Level::Block::Ramp::Left ){
                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 1 ].position.y + heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z;

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 2 ].position.y;
                verts[ v ].position.z = verts[ v - 2 ].position.z;

                v++;

                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension + blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 1 ].position.y;
                verts[ v ].position.z = verts[ v - 1 ].position.z;

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 2 ].position.y + heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z;

                v++;

                DungeonVertex::createSurfaceNormals( verts[v - 6], verts[v - 5], verts[v - 4] ); 
                DungeonVertex::createSurfaceNormals( verts[v - 3], verts[v - 2], verts[v - 1] ); 

                vertexCount += 6;
                mRampCount++;
            }else if( level.getBlockRamp(i,j) == Level::Block::Ramp::Right ){
                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval + heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension + blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x;
                verts[ v ].position.y = verts[ v - 1 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z;

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 2 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z;

                v++;

                verts[ v ].position.x = static_cast<float>(i) * blockDimension;
                verts[ v ].position.y = static_cast<float>(level.getBlockHeight(i, j)) * heightInterval + heightInterval;
                verts[ v ].position.z = static_cast<float>(j) * blockDimension;

                v++;

                verts[ v ].position.x = verts[ v - 1 ].position.x + blockDimension;
                verts[ v ].position.y = verts[ v - 1 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 1 ].position.z;

                v++;

                verts[ v ].position.x = verts[ v - 2 ].position.x;
                verts[ v ].position.y = verts[ v - 2 ].position.y - heightInterval;
                verts[ v ].position.z = verts[ v - 2 ].position.z;

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
