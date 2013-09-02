#include "EnvironmentDisplay.h"
#include "Log.h"
#include "Utils.h"

EnvironmentDisplay::EnvironmentDisplay() : mInputLayout(NULL),
    mRoomVB(NULL),
    mRoomIB(NULL),
    mBlockVB(NULL),
    mBlockIB(NULL),
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
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//Create the input layout
    D3DX11_PASS_DESC passDesc;
    tech->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Environment Vertex Input Layout" << LOG_ENDL;
            return false;
    }

    //Fill Vertex and Index Buffers
    float bottom[3] = {1.0f, 0.0f, 0.0f};
    float middle[3] = {0.0f, 1.0f, 0.0f};
    float top[3] = {0.0f, 0.0f, 1.0f};
    float wall[3] = {1.0f, 0.0f, 1.0f};

    float start = 0.0f;

    //Fill in vertex and index buffers
    EnvVertex vertices[] =
    {
		{ XMFLOAT3(start, start, start), bottom },
		{ XMFLOAT3(start, mHeightInterval, start), bottom },
		{ XMFLOAT3(mBlockDimension, mHeightInterval, start), bottom },
		{ XMFLOAT3(mBlockDimension, start, start), bottom },
		{ XMFLOAT3(start, start, mBlockDimension), bottom },
		{ XMFLOAT3(start, mHeightInterval, mBlockDimension), bottom },
		{ XMFLOAT3(mBlockDimension, mHeightInterval, mBlockDimension), bottom },
		{ XMFLOAT3(mBlockDimension, start, mBlockDimension), bottom },

        { XMFLOAT3(start, start, start), middle },
		{ XMFLOAT3(start, 2.0f * mHeightInterval, start), middle },
		{ XMFLOAT3(mBlockDimension, 2.0f * mHeightInterval, start), middle },
		{ XMFLOAT3(mBlockDimension, start, start), middle },
		{ XMFLOAT3(start, start, mBlockDimension), middle },
		{ XMFLOAT3(start, 2.0f * mHeightInterval, mBlockDimension), middle },
		{ XMFLOAT3(mBlockDimension, 2.0f * mHeightInterval, mBlockDimension), middle },
		{ XMFLOAT3(mBlockDimension, start, mBlockDimension), middle },

        { XMFLOAT3(start, start, start), top },
		{ XMFLOAT3(start, 3.0f * mHeightInterval, start), top },
		{ XMFLOAT3(mBlockDimension, 3.0f * mHeightInterval, start), top },
		{ XMFLOAT3(mBlockDimension, start, start), top },
		{ XMFLOAT3(start, start, mBlockDimension), top },
		{ XMFLOAT3(start, 3.0f * mHeightInterval, mBlockDimension), top },
		{ XMFLOAT3(mBlockDimension, 3.0f * mHeightInterval, mBlockDimension), top },
		{ XMFLOAT3(mBlockDimension, start, mBlockDimension), top },

        { XMFLOAT3(start, start, start), wall },
		{ XMFLOAT3(start, 4.0f * mHeightInterval, start), wall },
		{ XMFLOAT3(mBlockDimension, 4.0f * mHeightInterval, start), wall },
		{ XMFLOAT3(mBlockDimension, start, start), wall },
		{ XMFLOAT3(start, start, mBlockDimension), wall },
		{ XMFLOAT3(start, 4.0f * mHeightInterval, mBlockDimension), wall },
		{ XMFLOAT3(mBlockDimension, 4.0f * mHeightInterval, mBlockDimension), wall },
		{ XMFLOAT3(mBlockDimension, start, mBlockDimension), wall }
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(EnvVertex) * 32;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

    if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mBlockVB))){
        return false;
    }

	// Create the index buffer
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


        // front face
		8, 9, 10,
		8, 10, 11,

		// back face
		12, 14, 13,
		12, 15, 14,

		// left face
		12, 13, 9,
		12, 9, 8,

		// right face
		11, 10, 14,
		11, 14, 15,

		// top face
		9, 13, 14,
		9, 14, 10,


        // front face
		16, 17, 18,
		16, 18, 19,

		// back face
		20, 22, 21,
		20, 23, 22,

		// left face
		20, 21, 17,
		20, 17, 16,

		// right face
		19, 18, 22,
		19, 22, 23,

		// top face
		17, 21, 22,
		17, 22, 18,

        // front face
		24, 25, 26,
		24, 26, 27,

		// back face
		28, 30, 29,
		28, 31, 30,

		// left face
		28, 29, 25,
		28, 25, 24,

		// right face
		27, 26, 30,
		27, 30, 31,

		// top face
		25, 29, 30,
		25, 30, 26,
	};

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * 120;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    if(FAILED(device->CreateBuffer(&ibd, &iinitData, &mBlockIB))){
        return false;
    }

    return true;
}

bool EnvironmentDisplay::createRoomMesh( ID3D11Device* device, Environment::Room& room )
{
    //Room sides are made up of 4 rectangles that could surround a door
    //Top is a single rectangle

    ReleaseCOM( mRoomIB );
    ReleaseCOM( mRoomVB );

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
    float h = static_cast<float>(room.getExitHeight( Environment::Room::Exit::Front ) + 1) * mHeightInterval;
    float l = static_cast<float>(room.getExitLocation( Environment::Room::Exit::Front )) * mBlockDimension;

    if( l >= 0.1f ){ //There is no door if h is 0
        backDoor[0] = l; backDoor[2] = l + mBlockDimension;
        backDoor[1] = h; backDoor[3] = h + mDoorHeight;
    }

    h = static_cast<float>(room.getExitHeight( Environment::Room::Exit::Left ) + 1) * mHeightInterval;
    l = static_cast<float>(room.getExitLocation( Environment::Room::Exit::Left )) * mBlockDimension;

    if( l >= 0.1f ){ //There is no door if h is 0
        rightDoor[0] = l; rightDoor[2] = l + mBlockDimension;
        rightDoor[1] = h; rightDoor[3] = h + mDoorHeight;
    }

    h = static_cast<float>(room.getExitHeight( Environment::Room::Exit::Back ) + 1) * mHeightInterval;
    l = static_cast<float>(room.getExitLocation( Environment::Room::Exit::Back )) * mBlockDimension;

    if( l >= 0.1f ){ //There is no door if h is 0
        frontDoor[0] = l; frontDoor[2] = l + mBlockDimension;
        frontDoor[1] = h; frontDoor[3] = h + mDoorHeight;
    }

    h = static_cast<float>(room.getExitHeight( Environment::Room::Exit::Right ) + 1) * mHeightInterval;
    l = static_cast<float>(room.getExitLocation( Environment::Room::Exit::Right )) * mBlockDimension;

    if( l >= 0.1f ){ //There is no door if h is 0
        leftDoor[0] = l; leftDoor[2] = l + mBlockDimension;
        leftDoor[1] = h; leftDoor[3] = h + mDoorHeight;
    }

    EnvVertex vertices[] =
    {
        //Back Face 
        //Bottom Strip
        { XMFLOAT3(backStart, backStart, backStart), wallColor },
		{ XMFLOAT3(backStart, backDoor[1], backStart), wallColor },
        { XMFLOAT3(fullWidth, backDoor[1], backStart), wallColor },
        { XMFLOAT3(fullWidth, backStart, backStart), wallColor },

        //Top Strip
        { XMFLOAT3(backStart, backDoor[3], backStart), wallColor },
		{ XMFLOAT3(backStart, fullHeight, backStart), wallColor },
        { XMFLOAT3(fullWidth, fullHeight, backStart), wallColor },
        { XMFLOAT3(fullWidth, backDoor[3], backStart), wallColor },

        //Door
        { XMFLOAT3(backDoor[2], backDoor[1], backStart), wallColor },
		{ XMFLOAT3(backDoor[2], backDoor[3], backStart), wallColor },
        { XMFLOAT3(backDoor[0], backDoor[3], backStart), wallColor },
        { XMFLOAT3(backDoor[0], backDoor[1], backStart), wallColor },

        
        //Right Face
        //Bottom Strip
        { XMFLOAT3(backStart, backStart, frontStart), wallColor },
		{ XMFLOAT3(backStart, rightDoor[1], frontStart), wallColor },
        { XMFLOAT3(backStart, rightDoor[1], backStart), wallColor },
        { XMFLOAT3(backStart, backStart, backStart), wallColor },

        //Top Strip
        { XMFLOAT3(backStart, rightDoor[3], frontStart), wallColor },
		{ XMFLOAT3(backStart, fullHeight, frontStart), wallColor },
        { XMFLOAT3(backStart, fullHeight, backStart), wallColor },
        { XMFLOAT3(backStart, rightDoor[3], backStart), wallColor },

        //Door
        { XMFLOAT3(backStart, rightDoor[1], rightDoor[0]), wallColor },
		{ XMFLOAT3(backStart, rightDoor[3], rightDoor[0]), wallColor },
        { XMFLOAT3(backStart, rightDoor[3], rightDoor[2]), wallColor },
        { XMFLOAT3(backStart, rightDoor[1], rightDoor[2]), wallColor },


        //Front Face 
        //Bottom Strip
        { XMFLOAT3(backStart, backStart, frontStart), wallColor },
		{ XMFLOAT3(backStart, backDoor[1], frontStart), wallColor },
        { XMFLOAT3(fullWidth, backDoor[1], frontStart), wallColor },
        { XMFLOAT3(fullWidth, backStart, frontStart), wallColor },

        //Top Strip
        { XMFLOAT3(backStart, backDoor[3], frontStart), wallColor },
		{ XMFLOAT3(backStart, fullHeight, frontStart), wallColor },
        { XMFLOAT3(fullWidth, fullHeight, frontStart), wallColor },
        { XMFLOAT3(fullWidth, backDoor[3], frontStart), wallColor },

        //Door
        { XMFLOAT3(backDoor[2], backDoor[1], frontStart), wallColor },
		{ XMFLOAT3(backDoor[2], backDoor[3], frontStart), wallColor },
        { XMFLOAT3(backDoor[0], backDoor[3], frontStart), wallColor },
        { XMFLOAT3(backDoor[0], backDoor[1], frontStart), wallColor },

        //Left Face
        //Bottom Strip
        { XMFLOAT3(fullWidth, backStart, frontStart), wallColor },
		{ XMFLOAT3(fullWidth, leftDoor[1], frontStart), wallColor },
        { XMFLOAT3(fullWidth, leftDoor[1], backStart), wallColor },
        { XMFLOAT3(fullWidth, backStart, backStart), wallColor },

        //Top Strip
        { XMFLOAT3(fullWidth, leftDoor[3], frontStart), wallColor },
		{ XMFLOAT3(fullWidth, fullHeight, frontStart), wallColor },
        { XMFLOAT3(fullWidth, fullHeight, backStart), wallColor },
        { XMFLOAT3(fullWidth, leftDoor[3], backStart), wallColor },

        //Door
        { XMFLOAT3(fullWidth, leftDoor[1], leftDoor[0]), wallColor },
		{ XMFLOAT3(fullWidth, leftDoor[3], leftDoor[0]), wallColor },
        { XMFLOAT3(fullWidth, leftDoor[3], leftDoor[2]), wallColor },
        { XMFLOAT3(fullWidth, leftDoor[1], leftDoor[2]), wallColor },
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(EnvVertex) * ( (16 * 4) + 4);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

    if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mRoomVB))){
        return false;
    }

#define ROOM_INDICIES ( ( 4 * (3 * 8) ) )

    UINT indices[ ROOM_INDICIES ];

    UINT v = 0;
    int s = 0;

    //Gen indices for each side
    for(int i = 0; i < ROOM_INDICIES; ){
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
    ibd.ByteWidth = sizeof(UINT) * ROOM_INDICIES;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    if(FAILED(device->CreateBuffer(&ibd, &iinitData, &mRoomIB))){
        return false;
    }

    return true;
}

void EnvironmentDisplay::clear()
{
    ReleaseCOM( mInputLayout );
    ReleaseCOM( mBlockVB );
    ReleaseCOM( mBlockIB );
    ReleaseCOM( mRoomVB );
    ReleaseCOM( mRoomIB );
}

void EnvironmentDisplay::draw( ID3D11DeviceContext* device, Environment& env, ID3DX11Effect* fx, ID3DX11EffectTechnique* tech )
{
    device->IASetInputLayout( mInputLayout );
    device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    device->IASetIndexBuffer( mBlockIB, DXGI_FORMAT_R32_UINT, 0 );

    UINT stride = sizeof(EnvVertex);
    UINT offset = 0;
    device->IASetVertexBuffers(0, 1, &mBlockVB, &stride, &offset);
    XMMATRIX world = XMMatrixIdentity();
    XMMATRIX viewProj = XMMatrixIdentity();
    XMMATRIX worldViewProj = XMMatrixIdentity();

    ID3DX11EffectMatrixVariable* mfxViewProj = fx->GetVariableByName("gWorldViewProj")->AsMatrix();
    mfxViewProj->GetMatrix( (float*)&viewProj );

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );

    for(byte i = 0; i < env.getRoom().getWidth(); i++){
        for(byte j = 0; j < env.getRoom().getDepth(); j++){
            world = XMMatrixTranslation( i * 0.1f, 0.0f, j * 0.1f );
            worldViewProj = world * viewProj;
            mfxViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

            for(UINT p = 0; p < techDesc.Passes; ++p){
                tech->GetPassByIndex(p)->Apply(0, device);
                device->DrawIndexed(30, ( env.getRoom().getBlockHeight(i, j) * 30 ), 0);
            }
        }
    }

    world = XMMatrixIdentity();

    worldViewProj = world * viewProj;
    mfxViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

    device->IASetIndexBuffer( mRoomIB, DXGI_FORMAT_R32_UINT, 0 );
    device->IASetVertexBuffers(0, 1, &mRoomVB, &stride, &offset);

    for(UINT p = 0; p < techDesc.Passes; ++p){
        tech->GetPassByIndex(p)->Apply(0, device);
        device->DrawIndexed(ROOM_INDICIES, 0, 0);
    }
}