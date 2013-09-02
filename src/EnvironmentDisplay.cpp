#include "EnvironmentDisplay.h"
#include "Log.h"
#include "Utils.h"

EnvironmentDisplay::EnvironmentDisplay() : mInputLayout(NULL)
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

	// Create the input layout
    D3DX11_PASS_DESC passDesc;
    tech->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Environment Vertex Input Layout" << LOG_ENDL;
            return false;
    }

    return true;
}

void EnvironmentDisplay::clear()
{
    ReleaseCOM( mInputLayout );
}