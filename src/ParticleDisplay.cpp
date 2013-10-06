#include "ParticleDisplay.h"
#include "Log.h"

ParticleDisplay::ParticleDisplay()
{

}

bool ParticleDisplay::init(ID3D11Device* device, ID3DX11EffectTechnique* technique)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result;

	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//Create the input layout
    D3DX11_PASS_DESC passDesc;
    technique->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(device->CreateInputLayout(vertexDesc, 1, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Vertex Input Layout for Particles" << LOG_ENDL;
            return false;
    }

	// Create the vertex array for the particles that will be rendered.
	mVertices = new ParticleVertex[1];
	if(!mVertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(mVertices, 0, (sizeof(ParticleVertex)));

	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(ParticleVertex);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = mVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
	if(FAILED(result))
	{
		return false;
	}
	

	D3D11_BUFFER_DESC constDesc;
    ZeroMemory( &constDesc, sizeof( constDesc ) );
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof( ParticleInstanceData );
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mParticleInstanceDataCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for ParticleDisplay" << LOG_ENDL;
        return false;
    }

	return true;
}

void ParticleDisplay::clear()
{
    ReleaseCOM(mInputLayout);
    ReleaseCOM(mParticleInstanceDataCB);
    ReleaseCOM(mVertexBuffer);

    if( mVertices ){
        delete[] mVertices;
        mVertices = NULL;
    }
}

void ParticleDisplay::UpdateBuffers(ParticleInstanceData* pos, ID3D11DeviceContext* device)
{
	
	device->UpdateSubresource( mParticleInstanceDataCB, 0, 0, pos, 0, 0 );

}

void ParticleDisplay::Draw(ParticleInstanceData* pos, ID3D11DeviceContext* device, ID3D11ShaderResourceView* texture)
{
	UpdateBuffers(pos, device);

	UINT stride = sizeof(ParticleVertex);
    UINT offset = 0;

	//Set input layout and topology
    device->IASetInputLayout( mInputLayout );
	device->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	device->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );

	device->VSSetConstantBuffers(4, 1, &mParticleInstanceDataCB);
	device->PSSetShaderResources(0, 1, &texture);

	device->DrawInstanced(1, MAX_PARTICLES, 0, 0);
}