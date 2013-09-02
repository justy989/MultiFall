#include "EnvironmentDisplay.h"
#include "Log.h"
#include "Utils.h"

#include "Environment.h"

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

    float start = 0.0f;

    //Fill in vertex and index buffers
    EnvVertex vertices[] =
    {
		{ XMFLOAT3(start, start, start), bottom },
		{ XMFLOAT3(start, +0.1f, start), bottom },
		{ XMFLOAT3(+0.1f, +0.1f, start), bottom },
		{ XMFLOAT3(+0.1f, start, start), bottom },
		{ XMFLOAT3(start, start, +0.1f), bottom },
		{ XMFLOAT3(start, +0.1f, +0.1f), bottom },
		{ XMFLOAT3(+0.1f, +0.1f, +0.1f), bottom },
		{ XMFLOAT3(+0.1f, start, +0.1f), bottom },

        { XMFLOAT3(start, start, start), middle },
		{ XMFLOAT3(start, +0.2f, start), middle },
		{ XMFLOAT3(+0.1f, +0.2f, start), middle },
		{ XMFLOAT3(+0.1f, start, start), middle },
		{ XMFLOAT3(start, start, +0.1f), middle },
		{ XMFLOAT3(start, +0.2f, +0.1f), middle },
		{ XMFLOAT3(+0.1f, +0.2f, +0.1f), middle },
		{ XMFLOAT3(+0.1f, start, +0.1f), middle },

        { XMFLOAT3(start, start, start), top },
		{ XMFLOAT3(start, +0.3f, start), top },
		{ XMFLOAT3(+0.1f, +0.3f, start), top },
		{ XMFLOAT3(+0.1f, start, start), top },
		{ XMFLOAT3(start, start, +0.1f), top },
		{ XMFLOAT3(start, +0.3f, +0.1f), top },
		{ XMFLOAT3(+0.1f, +0.3f, +0.1f), top },
		{ XMFLOAT3(+0.1f, start, +0.1f), top }
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(EnvVertex) * 24;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

    if(FAILED(device->CreateBuffer(&vbd, &vinitData, &mEnvVB))){
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
	};

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * 90;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    if(FAILED(device->CreateBuffer(&ibd, &iinitData, &mEnvIB))){
        return false;
    }

    return true;
}

void EnvironmentDisplay::clear()
{
    ReleaseCOM( mInputLayout );
}

void EnvironmentDisplay::draw( ID3D11DeviceContext* device, Environment& env, ID3DX11Effect* fx, ID3DX11EffectTechnique* tech )
{
    device->IASetInputLayout( mInputLayout );
    device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    device->IASetIndexBuffer( mEnvIB, DXGI_FORMAT_R32_UINT, 0 );

    UINT stride = sizeof(EnvVertex);
    UINT offset = 0;
    device->IASetVertexBuffers(0, 1, &mEnvVB, &stride, &offset);
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
}