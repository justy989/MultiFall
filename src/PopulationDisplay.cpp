#include "PopulationDisplay.h"

#include "Log.h"

PopulationDisplay::PopulationDisplay() :
    mInputLayout(NULL),
	mBillboardCount(0),
	mBillboardBuffer(NULL),
	mInstanceDataCB(NULL),
    mDrawRange( 10.0f )
{
    mTextures[0] = NULL;
}

PopulationDisplay::~PopulationDisplay()
{
    clear();
}

bool PopulationDisplay::init( ID3D11Device* device, ID3DX11EffectTechnique* technique )
{
    D3D11_INPUT_ELEMENT_DESC billboardDesc[5] = 
    {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"DIMENSION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    //{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    //{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    //{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    //{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

	//Create the input layout
    D3DX11_PASS_DESC passDesc;
    technique->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(device->CreateInputLayout(billboardDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Vertex Input Layout for Population" << LOG_ENDL;
            return false;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(CharacterBillboard) * POPULATION_MAX_CHARACTERS;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    if(FAILED(device->CreateBuffer(&vbd, NULL, &mBillboardBuffer))){
        LOG_ERRO << "Unable to allocate Vertex buffer for billboard" << LOG_INFO;
        return false;
    }

    HRESULT hr = D3DX11CreateShaderResourceViewFromFile( device,
        L"content/textures/skeleton.png", 0, 0, &mTextures[0], 0 );

    if( FAILED(hr) ){
        LOG_ERRO << "Unable to load Character texture: " << L"content/textures/skeleton.png" << LOG_ENDL;
        return false;
    }

    return true;
}

void PopulationDisplay::clear()
{
    ReleaseCOM( mInputLayout );
    ReleaseCOM( mBillboardBuffer );
    ReleaseCOM( mInstanceDataCB );
    ReleaseCOM( mTextures[0] );
}

void PopulationDisplay::draw( ID3D11DeviceContext* device )
{    
    uint stride = sizeof( CharacterBillboard );
    uint offset = 0;

    device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
    device->IASetInputLayout( mInputLayout );
    device->IASetVertexBuffers( 0, 1, &mBillboardBuffer, &stride, &offset );

    device->PSSetShaderResources( 0, 1, &mTextures[0] );
    device->Draw(mBillboardCount, 0);
}

void PopulationDisplay::updateBillboards( ID3D11DeviceContext* device, World& world, XMFLOAT4& cameraPos )
{
    mBillboardCount = 0;

    Population& p = world.getPopulation();
    
	D3D11_MAPPED_SUBRESOURCE mappedData;
    if( FAILED(device->Map(mBillboardBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData) ) ){
        return;
    }

    CharacterBillboard* billboards = (CharacterBillboard*)(mappedData.pData);

    for(int i = 0; i < POPULATION_MAX_CHARACTERS; i++){
        if( p.getCharacter(i).getExistence() == WorldEntity::Existence::Alive ){

            float dx = (cameraPos.x - p.getCharacter(i).getPosition().x);
            float dz = (cameraPos.z - p.getCharacter(i).getPosition().z);

            float d = sqrt( (dx * dx) + (dz * dz) );

            //If we are outside the draw range, skip drawing this one
            if( d > mDrawRange ){
                continue;
            }

            billboards[mBillboardCount].pos = p.getCharacter(i).getPosition();
            billboards[mBillboardCount].dimensions.x = 0.1f;
            billboards[mBillboardCount].dimensions.y = 0.25f;
            billboards[mBillboardCount].dimensions.z = 0.0f;
            billboards[mBillboardCount].dimensions.w = 0.0f;
            mBillboardCount++;
        }
    }

    device->Unmap( mBillboardBuffer, 0 );
}