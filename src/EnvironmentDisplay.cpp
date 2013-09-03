#include "EnvironmentDisplay.h"
#include "Log.h"
#include "Utils.h"

EnvironmentDisplay::EnvironmentDisplay() : mInputLayout(NULL),
    mFrameVB(NULL),
    mFrameIB(NULL),
    mFloorVB(NULL),
    mFloorIB(NULL),
    mWallsVB(NULL),
    mWallsIB(NULL),
    mHeightInterval( 0.3f ),
    mBlockDimension( 0.1f ),
    mDoorHeight( 0.2f )
{

}

bool EnvironmentDisplay::init( ID3D11Device* device, ID3DX11EffectTechnique* tech )
{
     // Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//Create the input layout
    D3DX11_PASS_DESC passDesc;
    tech->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Environment Vertex Input Layout" << LOG_ENDL;
            return false;
    }

    return true;
}

bool EnvironmentDisplay::createRoomMesh( ID3D11Device* device, Environment::Room& room )
{
    if( !createFrameMesh( device, room ) ){
        return false;
    }

    if( !createFloorMesh( device, room ) ){
        return false;
    }

    if( !createWallsMesh( device, room ) ){
        return false;
    }

    return true;
}

void EnvironmentDisplay::createSurfaceNormal( EnvVertex* a, EnvVertex* b, EnvVertex* c)
{
	XMFLOAT3 normal;
	///

	XMVECTOR v1 = XMLoadFloat3(&XMFLOAT3(a->position.x, a->position.y, a->position.z));
	XMVECTOR v2 = XMLoadFloat3(&XMFLOAT3(b->position.x, b->position.y, b->position.z));
	XMVECTOR v3 = XMLoadFloat3(&XMFLOAT3(c->position.x, c->position.y, c->position.z));
	XMVECTOR n  = XMVector3Cross(XMVectorSubtract(v2 ,v1), XMVectorSubtract(v3 ,v1));

	n = XMVector3Normalize(n);
	XMStoreFloat3(&normal, n);

	a->normal = normal;
	b->normal = normal;
	c->normal = normal;

	//LOG_INFO<<(double)normal.x<<" "<<(double)normal.y<<" "<<(double)normal.z<<LOG_ENDL;
}

bool EnvironmentDisplay::createFloorMesh( ID3D11Device* device, Environment::Room& room )
{
    ReleaseCOM( mFloorVB );
    ReleaseCOM( mFloorIB );

    mRoomSize = room.getWidth() * room.getDepth();

    EnvVertex* verts = new EnvVertex[ 4 * mRoomSize ];
    ushort* inds = new ushort[ 6 * mRoomSize ];

    int v = 0;

    float floorColors[4][4] = {
        {1.0f, 0.0f, 0.0f, 1.0f}, 
        {0.0f, 1.0f, 0.0f, 1.0f}, 
        {0.0f, 0.0f, 1.0f, 1.0f}, 
        {1.0f, 0.0f, 1.0f, 1.0f}
    };

    for(int i = 0; i < room.getWidth(); i++){
        for(int j = 0; j < room.getDepth(); j++){
            
            verts[ v ].position.x = static_cast<float>(i) * mBlockDimension;
            verts[ v ].position.y = static_cast<float>(room.getBlockHeight(i, j)) * mHeightInterval;
            verts[ v ].position.z = static_cast<float>(j) * mBlockDimension;
            verts[ v ].color.x = floorColors[ room.getBlockHeight(i, j) ][0];
            verts[ v ].color.y = floorColors[ room.getBlockHeight(i, j) ][1];
            verts[ v ].color.z = floorColors[ room.getBlockHeight(i, j) ][2];
            verts[ v ].color.w = floorColors[ room.getBlockHeight(i, j) ][3];

            v++;

            verts[ v ].position.x = verts[ v - 1 ].position.x + mBlockDimension;
            verts[ v ].position.y = verts[ v - 1 ].position.y;
            verts[ v ].position.z = verts[ v - 1 ].position.z;
            verts[ v ].color = verts[ v - 1 ].color;

            v++;

            verts[ v ].position.x = verts[ v - 2 ].position.x;
            verts[ v ].position.y = verts[ v - 2 ].position.y;
            verts[ v ].position.z = verts[ v - 2 ].position.z + mBlockDimension;
            verts[ v ].color = verts[ v - 2 ].color;

            v++;

            verts[ v ].position.x = verts[ v - 3 ].position.x + mBlockDimension;
            verts[ v ].position.y = verts[ v - 3 ].position.y;
            verts[ v ].position.z = verts[ v - 3 ].position.z + mBlockDimension;
            verts[ v ].color = verts[ v - 3 ].color;

            v++;

			createSurfaceNormal(&verts[v-4], &verts[v-2], &verts[v-3]);
			createSurfaceNormal(&verts[v-3], &verts[v-2], &verts[v-1]);
        }
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(EnvVertex) * (mRoomSize * 4);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = verts;

    if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mFloorVB))){
        LOG_ERRO << "Unable to allocate Vertex buffer for room floor" << LOG_INFO;
        return false;
    }

    //Reusing tmp variable for indexing index buffer!
    v = 0;

    int index = 0;

    for(int i = 0; i < room.getWidth(); i++){
        for(int j = 0; j < room.getDepth(); j++){
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
    ibd.ByteWidth = sizeof(ushort) * mRoomSize * 6;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = inds;

    if(FAILED(device->CreateBuffer(&ibd, &iinitData, &mFloorIB))){
        LOG_ERRO << "Unable to allocate index buffer for room frame" << LOG_INFO;
        return false;
    }

    delete[] verts;
    delete[] inds;

    return true;
}

bool EnvironmentDisplay::createWallsMesh( ID3D11Device* device, Environment::Room& room )
{
    ReleaseCOM( mWallsIB );
    ReleaseCOM( mWallsVB );

    int v = 0;
    int indexCount = 0;
    int vertexCount = 0;

    EnvVertex* verts = new EnvVertex[ 4 * 4 * mRoomSize ];

    float wallColor[4] = { 0.0f, 1.0f, 1.0f, 1.0f };

    for(int i = 0; i < room.getWidth(); i++){
        for(int j = 0; j < room.getDepth(); j++){ 

            //Make Left Wall
            int nextI = i - 1;
            int nextJ = j;

            if( nextI >= 0 ){
                if( room.getBlockHeight( i, j ) > room.getBlockHeight( nextI, nextJ ) ){
                    //Make Wall
                    verts[ v ].position.x = static_cast<float>(i) * mBlockDimension;
                    verts[ v ].position.y = static_cast<float>(room.getBlockHeight(i, j)) * mHeightInterval;
                    verts[ v ].position.z = static_cast<float>(j) * mBlockDimension;
                    verts[ v ].color.x = wallColor[0];
                    verts[ v ].color.y = wallColor[1];
                    verts[ v ].color.z = wallColor[2];

                    v++;

                    verts[ v ].position.x = verts[ v - 1 ].position.x;
                    verts[ v ].position.y = verts[ v - 1 ].position.y;
                    verts[ v ].position.z = verts[ v - 1 ].position.z + mBlockDimension;
                    verts[ v ].color = verts[ v - 1 ].color;

                    v++;

                    verts[ v ].position.x = verts[ v - 2 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 2 ].position.z;
                    verts[ v ].color = verts[ v - 2 ].color;

                    v++;

                    verts[ v ].position.x = verts[ v - 3 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 3 ].position.z + mBlockDimension;
                    verts[ v ].color = verts[ v - 3 ].color;

                    v++;

                    vertexCount += 4;
                    indexCount += 6;

					createSurfaceNormal(&verts[v-4], &verts[v-2], &verts[v-3]);
					createSurfaceNormal(&verts[v-3], &verts[v-2], &verts[v-1]);
                }
            }

            nextI = i + 1;
            nextJ = j;

            if( nextI < room.getWidth() ){
                if( room.getBlockHeight( i, j ) > room.getBlockHeight( nextI, nextJ ) ){
                    //Make Wall
                    verts[ v ].position.x = static_cast<float>(i) * mBlockDimension + mBlockDimension;
                    verts[ v ].position.y = static_cast<float>(room.getBlockHeight(i, j)) * mHeightInterval;
                    verts[ v ].position.z = static_cast<float>(j) * mBlockDimension + mBlockDimension;
                    verts[ v ].color.x = wallColor[0];
                    verts[ v ].color.y = wallColor[1];
                    verts[ v ].color.z = wallColor[2];

                    v++;
                    
                    verts[ v ].position.x = verts[ v - 1 ].position.x;
                    verts[ v ].position.y = verts[ v - 1 ].position.y;
                    verts[ v ].position.z = verts[ v - 1 ].position.z - mBlockDimension;
                    verts[ v ].color = verts[ v - 1 ].color;

                    v++;

                    verts[ v ].position.x = verts[ v - 2 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 2 ].position.z;
                    verts[ v ].color = verts[ v - 2 ].color;

                    v++;

                    verts[ v ].position.x = verts[ v - 3 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 3 ].position.z - mBlockDimension;
                    verts[ v ].color = verts[ v - 3 ].color;

                    v++;

                    vertexCount += 4;
                    indexCount += 6;

					createSurfaceNormal(&verts[v-4], &verts[v-2], &verts[v-3]);
					createSurfaceNormal(&verts[v-3], &verts[v-2], &verts[v-1]);
                }
            }

            nextI = i;
            nextJ = j - 1;

            if( nextJ >= 0 ){
                if( room.getBlockHeight( i, j ) > room.getBlockHeight( nextI, nextJ ) ){
                    //Make Wall
                    verts[ v ].position.x = static_cast<float>(i) * mBlockDimension + mBlockDimension;
                    verts[ v ].position.y = static_cast<float>(room.getBlockHeight(i, j)) * mHeightInterval;
                    verts[ v ].position.z = static_cast<float>(j) * mBlockDimension;
                    verts[ v ].color.x = wallColor[0];
                    verts[ v ].color.y = wallColor[1];
                    verts[ v ].color.z = wallColor[2];

                    v++;
                    
                    verts[ v ].position.x = verts[ v - 1 ].position.x - mBlockDimension;
                    verts[ v ].position.y = verts[ v - 1 ].position.y;
                    verts[ v ].position.z = verts[ v - 1 ].position.z;
                    verts[ v ].color = verts[ v - 1 ].color;

                    v++;

                    verts[ v ].position.x = verts[ v - 2 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 2 ].position.z;
                    verts[ v ].color = verts[ v - 2 ].color;

                    v++;

                    verts[ v ].position.x = verts[ v - 3 ].position.x - mBlockDimension;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 3 ].position.z;
                    verts[ v ].color = verts[ v - 3 ].color;

                    v++;

                    vertexCount += 4;
                    indexCount += 6;

					createSurfaceNormal(&verts[v-4], &verts[v-2], &verts[v-3]);
					createSurfaceNormal(&verts[v-3], &verts[v-2], &verts[v-1]);
                }
            }

            nextI = i;
            nextJ = j + 1;

            if( nextJ < room.getDepth() ){
                if( room.getBlockHeight( i, j ) > room.getBlockHeight( nextI, nextJ ) ){
                    //Make Wall
                    verts[ v ].position.x = static_cast<float>(i) * mBlockDimension;
                    verts[ v ].position.y = static_cast<float>(room.getBlockHeight(i, j)) * mHeightInterval;
                    verts[ v ].position.z = static_cast<float>(j) * mBlockDimension + mBlockDimension;
                    verts[ v ].color.x = wallColor[0];
                    verts[ v ].color.y = wallColor[1];
                    verts[ v ].color.z = wallColor[2];

                    v++;
                    
                    verts[ v ].position.x = verts[ v - 1 ].position.x + mBlockDimension;
                    verts[ v ].position.y = verts[ v - 1 ].position.y;
                    verts[ v ].position.z = verts[ v - 1 ].position.z;
                    verts[ v ].color = verts[ v - 1 ].color;

                    v++;

                    verts[ v ].position.x = verts[ v - 2 ].position.x;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 2 ].position.z;
                    verts[ v ].color = verts[ v - 2 ].color;

                    v++;

                    verts[ v ].position.x = verts[ v - 3 ].position.x + mBlockDimension;
                    verts[ v ].position.y = 0.0f;
                    verts[ v ].position.z = verts[ v - 3 ].position.z;
                    verts[ v ].color = verts[ v - 3 ].color;

                    v++;

                    vertexCount += 4;
                    indexCount += 6;

					createSurfaceNormal(&verts[v-4], &verts[v-2], &verts[v-3]);
					createSurfaceNormal(&verts[v-3], &verts[v-2], &verts[v-1]);
                }
            }
        }
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(EnvVertex) * vertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = verts;

    if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mWallsVB))){
        LOG_ERRO << "Unable to allocate Vertex buffer for room walls" << LOG_INFO;
        return false;
    }

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
        LOG_ERRO << "Unable to allocate index buffer for room walls" << LOG_INFO;
        return false;
    }

    mWallIndices = indexCount;

    return true;
}

bool EnvironmentDisplay::createFrameMesh( ID3D11Device* device, Environment::Room& room )
{
    //Room sides are made up of 4 rectangles that could surround a door
    //Top is a single rectangle

    ReleaseCOM( mFrameIB );
    ReleaseCOM( mFrameVB );

    float backStart = 0.0f;
    float halfWidth = ( static_cast<float>(room.getWidth()) * mBlockDimension ) / 2.0f;
    float halfDepth = ( static_cast<float>(room.getDepth()) * mBlockDimension ) / 2.0f;
    float halfHeight = ( static_cast<float>(room.getHeight() + 1) * mHeightInterval ) / 2.0f;

    float fullWidth = halfWidth * 2.0f;
    float fullHeight = halfHeight * 2.0f;
    float fullDepth = halfDepth * 2.0f;

    float frontStart = fullDepth;

    float wallColor[4] = { 1.0f, 1.0f, 0.0f };

    float frontDoor[4] = { 0.0f, halfHeight, 0.0f, halfHeight };
    float backDoor[4] = { 0.0f, halfHeight, 0.0f, halfHeight };
    float leftDoor[4] = { 0.0f, halfHeight, 0.0f, halfHeight };
    float rightDoor[4] = { 0.0f, halfHeight, 0.0f, halfHeight };

    //Get the height and location
    float h = static_cast<float>(room.getExitHeight( Environment::Room::Exit::Front ) ) * mHeightInterval;
    float l = static_cast<float>(room.getExitLocation( Environment::Room::Exit::Front )) * mBlockDimension;

    if( l >= 0.1f ){ //There is no door if h is 0
        backDoor[0] = l; backDoor[2] = l + mBlockDimension;
        backDoor[1] = h; backDoor[3] = h + mDoorHeight;
    }

    h = static_cast<float>(room.getExitHeight( Environment::Room::Exit::Left )) * mHeightInterval;
    l = static_cast<float>(room.getExitLocation( Environment::Room::Exit::Left )) * mBlockDimension;

    if( l >= 0.1f ){ //There is no door if h is 0
        rightDoor[0] = l; rightDoor[2] = l + mBlockDimension;
        rightDoor[1] = h; rightDoor[3] = h + mDoorHeight;
    }

    h = static_cast<float>(room.getExitHeight( Environment::Room::Exit::Back )) * mHeightInterval;
    l = static_cast<float>(room.getExitLocation( Environment::Room::Exit::Back )) * mBlockDimension;

    if( l >= 0.1f ){ //There is no door if h is 0
        frontDoor[0] = l; frontDoor[2] = l + mBlockDimension;
        frontDoor[1] = h; frontDoor[3] = h + mDoorHeight;
    }

    h = static_cast<float>(room.getExitHeight( Environment::Room::Exit::Right )) * mHeightInterval;
    l = static_cast<float>(room.getExitLocation( Environment::Room::Exit::Right )) * mBlockDimension;

    if( l >= 0.1f ){ //There is no door if h is 0
        leftDoor[0] = l; leftDoor[2] = l + mBlockDimension;
        leftDoor[1] = h; leftDoor[3] = h + mDoorHeight;
    }

    EnvVertex vertices[] =
    {
        //Back Face 
        //Bottom Strip
        { XMFLOAT3(backStart, backStart, backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(backStart, backDoor[1], backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(fullWidth, backDoor[1], backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(fullWidth, backStart, backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },

        //Top Strip
        { XMFLOAT3(backStart, backDoor[3], backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(backStart, fullHeight, backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(fullWidth, fullHeight, backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(fullWidth, backDoor[3], backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },

        //Door
        { XMFLOAT3(backDoor[2], backDoor[1], backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(backDoor[2], backDoor[3], backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(backDoor[0], backDoor[3], backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },
        { XMFLOAT3(backDoor[0], backDoor[1], backStart), wallColor, XMFLOAT3(0.0f, 0.0f, -1.0f) },

        
        //Right Face
        //Bottom Strip
        { XMFLOAT3(backStart, backStart, frontStart), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(backStart, rightDoor[1], frontStart), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },
        { XMFLOAT3(backStart, rightDoor[1], backStart), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },
        { XMFLOAT3(backStart, backStart, backStart), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },

        //Top Strip
        { XMFLOAT3(backStart, rightDoor[3], frontStart), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },
		{ XMFLOAT3(backStart, fullHeight, frontStart), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },
        { XMFLOAT3(backStart, fullHeight, backStart), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },
        { XMFLOAT3(backStart, rightDoor[3], backStart), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },

        //Door
        { XMFLOAT3(backStart, rightDoor[1], rightDoor[0]), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },
		{ XMFLOAT3(backStart, rightDoor[3], rightDoor[0]), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },
        { XMFLOAT3(backStart, rightDoor[3], rightDoor[2]), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },
        { XMFLOAT3(backStart, rightDoor[1], rightDoor[2]), wallColor, XMFLOAT3(1.0f, 0.0f, 0.0f)  },


        //Front Face 
        //Bottom Strip
        { XMFLOAT3(backStart, backStart, frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(backStart, backDoor[1], frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(fullWidth, backDoor[1], frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(fullWidth, backStart, frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },

        //Top Strip
        { XMFLOAT3(backStart, backDoor[3], frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(backStart, fullHeight, frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(fullWidth, fullHeight, frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(fullWidth, backDoor[3], frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },

        //Door
        { XMFLOAT3(backDoor[2], backDoor[1], frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(backDoor[2], backDoor[3], frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(backDoor[0], backDoor[3], frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(backDoor[0], backDoor[1], frontStart), wallColor, XMFLOAT3(0.0f, 0.0f, 1.0f) },

        //Left Face
        //Bottom Strip
        { XMFLOAT3(fullWidth, backStart, frontStart), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(fullWidth, leftDoor[1], frontStart), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(fullWidth, leftDoor[1], backStart), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(fullWidth, backStart, backStart), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },

        //Top Strip
        { XMFLOAT3(fullWidth, leftDoor[3], frontStart), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(fullWidth, fullHeight, frontStart), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(fullWidth, fullHeight, backStart), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(fullWidth, leftDoor[3], backStart), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },

        //Door
        { XMFLOAT3(fullWidth, leftDoor[1], leftDoor[0]), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(fullWidth, leftDoor[3], leftDoor[0]), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(fullWidth, leftDoor[3], leftDoor[2]), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(fullWidth, leftDoor[1], leftDoor[2]), wallColor, XMFLOAT3(-1.0f, 0.0f, 0.0f) },
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(EnvVertex) * ( (12 * 4) + 4);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

    if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mFrameVB))){
        LOG_ERRO << "Unable to allocate Vertex buffer for room frame" << LOG_INFO;
        return false;
    }

    ushort indices[ ROOM_INDEX_COUNT ];

    ushort v = 0;
    int s = 0;

    //Gen indices for each side
    for(int i = 0; i < ROOM_INDEX_COUNT; ){
        //Bottom Section
        if( s < 2 ){
            indices[i] = v; indices[i+1] = v + 2; indices[i+2] = v + 1;
            indices[i+3] = v; indices[i+4] = v + 3; indices[i+5] = v + 2;
        }else{ //We need to gen indices in a different direction
            indices[i] = v; indices[i+1] = v + 1; indices[i+2] = v + 2;
            indices[i+3] = v; indices[i+4] = v + 2; indices[i+5] = v + 3;
        }

        v += 4;
        i += 6;

        //Top Section
        if( s < 2 ){
            indices[i] = v;
            indices[i+1] = v + 2;
            indices[i+2] = v + 1;

            indices[i+3] = v;
            indices[i+4] = v + 3;
            indices[i+5] = v + 2;
        }else{
            indices[i] = v;
            indices[i+1] = v + 1;
            indices[i+2] = v + 2;

            indices[i+3] = v;
            indices[i+4] = v + 2;
            indices[i+5] = v + 3;
        }

        v += 4;
        i += 6;

        //Left Middle Strip
        if( s < 2 ){
            indices[i] = v; 
            indices[i+1] = v - 1;
            indices[i+2] = v + 1;

            indices[i+3] = v;
            indices[i+4] = v - 6;
            indices[i+5] = v - 1;
        }else{
            indices[i] = v; 
            indices[i+1] = v + 1;
            indices[i+2] = v - 1;

            indices[i+3] = v;
            indices[i+4] = v - 1;
            indices[i+5] = v - 6;
        }

        //v += 4;
        i += 6;

        //Right Middle Strip
        if( s < 2 ){
            indices[i] = v + 2; 
            indices[i+1] = v - 7;
            indices[i+2] = v + 3;

            indices[i+3] = v + 2;
            indices[i+4] = v - 4;
            indices[i+5] = v - 7;
        }else{
            indices[i] = v + 2; 
            indices[i+1] = v + 3;
            indices[i+2] = v - 7;

            indices[i+3] = v + 2;
            indices[i+4] = v - 7;
            indices[i+5] = v - 4;
        }

        v += 4;
        i += 6;

        s++;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(ushort) * ROOM_INDEX_COUNT;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    if(FAILED(device->CreateBuffer(&ibd, &iinitData, &mFrameIB))){
        LOG_ERRO << "Unable to allocate index buffer for room frame" << LOG_INFO;
        return false;
    }

    return true;
}

void EnvironmentDisplay::clear()
{
    ReleaseCOM( mInputLayout );
    ReleaseCOM( mFrameVB );
    ReleaseCOM( mFrameIB );
    ReleaseCOM( mFloorVB );
    ReleaseCOM( mFloorIB );
    ReleaseCOM( mWallsVB );
    ReleaseCOM( mWallsVB );
}

void EnvironmentDisplay::draw( ID3D11DeviceContext* device, Environment& env, ID3DX11Effect* fx, ID3DX11EffectTechnique* tech )
{
    device->IASetInputLayout( mInputLayout );
    device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    UINT stride = sizeof(EnvVertex);
    UINT offset = 0;

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );

    for(ushort p = 0; p < techDesc.Passes; ++p){
        tech->GetPassByIndex(p)->Apply(0, device);

        device->IASetIndexBuffer( mFloorIB, DXGI_FORMAT_R16_UINT, 0 );
        device->IASetVertexBuffers(0, 1, &mFloorVB, &stride, &offset);

        device->DrawIndexed(6 * mRoomSize, 0, 0);

        device->IASetIndexBuffer( mWallsIB, DXGI_FORMAT_R16_UINT, 0 );
        device->IASetVertexBuffers(0, 1, &mWallsVB, &stride, &offset);

        device->DrawIndexed(mWallIndices, 0, 0);

        device->IASetIndexBuffer( mFrameIB, DXGI_FORMAT_R16_UINT, 0 );
        device->IASetVertexBuffers(0, 1, &mFrameVB, &stride, &offset);

        device->DrawIndexed(ROOM_INDEX_COUNT, 0, 0);
    }
}