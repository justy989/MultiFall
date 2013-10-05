#include "PopulationDisplay.h"

#include "Log.h"

PopulationDisplay::PopulationDisplay() :
    mInputLayout(NULL),
	mBillboardCount(0),
	mBillboardBuffer(NULL),
	mInstanceDataCB(NULL)
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

void PopulationDisplay::updateBillboards( ID3D11DeviceContext* device, World& world )
{
    mBillboardCount = 0;

    Population& p = world.getPopulation();

    for(int i = 0; i < POPULATION_MAX_CHARACTERS; i++){
        if( p.getCharacter(i).getExistence() == WorldEntity::Existence::Alive ){
            mBillboards[mBillboardCount].pos = p.getCharacter(i).getPosition();
            mBillboards[mBillboardCount].dimensions.x = 0.1f;
            mBillboards[mBillboardCount].dimensions.y = 0.25f;
            mBillboards[mBillboardCount].dimensions.z = 0.0f;
            mBillboards[mBillboardCount].dimensions.w = 0.0f;
            mBillboardCount++;
        }
    }
    
	D3D11_MAPPED_SUBRESOURCE mappedData;
    if( FAILED(device->Map(mBillboardBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData) ) ){
        return;
    }

    memcpy( mappedData.pData, mBillboards, sizeof(CharacterBillboard) * mBillboardCount);

    device->Unmap( mBillboardBuffer, 0 );
}