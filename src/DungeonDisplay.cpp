#include "DungeonDisplay.h"
#include "Log.h"
#include "Utils.h"

DungeonDisplay::DungeonDisplay() : 
    mInputLayout(NULL),
    mHeightInterval(0.3f),
    mBlockDimension(0.3f),
    mTextureSampler(NULL)
{

}

bool DungeonDisplay::init( ID3D11Device* device, ID3DX11EffectTechnique* tech )
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
    tech->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Dungeon Vertex Input Layout" << LOG_ENDL;
            return false;
    }

    //Describe and create the Texture Sampler
	D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    if( FAILED(device->CreateSamplerState( &samplerDesc, &mTextureSampler ) ) ){
        LOG_ERRO << "Failed to create Texture Sampler for Dungeon" << LOG_ENDL;
        return false;
    }

    LOG_INFO << "Created Input Description and Texture Sampler for Dungeon" << LOG_ENDL;

    //TMP: Create textures for level
    mLevelDisplay.setTextures( device, L"content/textures/stonefloorsheet1.png", 0.25f, L"content/textures/stonewall1.png", 1.0f );
    return true;
}

bool DungeonDisplay::createMeshFromLevel( ID3D11Device* device, Level& level )
{
    return mLevelDisplay.createMeshFromLevel( device, level, mBlockDimension, mHeightInterval );
}

void DungeonDisplay::clear()
{
    ReleaseCOM( mInputLayout );
    ReleaseCOM( mTextureSampler );

    mLevelDisplay.clear();
}

void DungeonDisplay::draw( ID3D11DeviceContext* device, ID3DX11EffectTechnique* tech )
{
    device->IASetInputLayout( mInputLayout );
    device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    mLevelDisplay.draw( device, tech, mTextureSampler );
}

void DungeonDisplay::drawLights( ID3D11DeviceContext* device, XMMATRIX* ViewProj )
{
	mLevelDisplay.drawLights(device, ViewProj);
}