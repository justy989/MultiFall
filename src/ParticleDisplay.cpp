#include "ParticleDisplay.h"
#include "Log.h"

ParticleDisplay::ParticleDisplay()
{

}

bool ParticleDisplay::init(ID3D11Device* device, ID3DX11EffectTechnique* technique)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//Create the input layout
    D3DX11_PASS_DESC passDesc;
    technique->GetPassByIndex(0)->GetDesc( &passDesc );

    if(FAILED(device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout))){
            LOG_ERRO << "Failed to create Vertex Input Layout for Particles" << LOG_ENDL;
            return false;
    }

	// Create the vertex array for the particles that will be rendered.
	mVertices = new ParticleVertex[mVertexCount];
	if(!mVertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(mVertices, 0, (sizeof(ParticleVertex)));

	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(ParticleVertex) * mVertexCount;
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

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete [] indices;
	indices = 0;

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

void ParticleDisplay::UpdateBuffers(ParticleInstanceData* pos, ID3D11DeviceContext* device)
{
	int index, i;
	HRESULT result;	

	device->UpdateSubresource( mParticleInstanceDataCB, 0, 0, pos, 0, 0 );
}

void ParticleDisplay::Draw(ParticleInstanceData* pos, ID3D11DeviceContext* device)
{
	UpdateBuffers(pos, device);

	//Set input layout and topology
    device->IASetInputLayout( mInputLayout );
	device->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );

	device->VSSetConstantBuffers(4, 1, &mParticleInstanceDataCB);
	device->GSSetConstantBuffers(4, 1, &mParticleInstanceDataCB);
}