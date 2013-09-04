#include "TextManager.h"

TextManager::TextManager(ID3D11DeviceContext* device)
{
	ID3D11Device* tDevice = NULL;
	device->GetDevice(&tDevice);

	ID3DBlob* vsBuffer = 0;

	CompileD3DShader( L"texture.fx", "VS_Main", "vs_4_0", &vsBuffer );

	tDevice->CreateVertexShader( vsBuffer->GetBufferPointer( ),
	vsBuffer->GetBufferSize( ), 0, &mVertexShader );	 

	ID3DBlob* psBuffer = 0;

	CompileD3DShader( L"texture.fx", "PS_Main", "ps_4_0", &psBuffer );

	tDevice->CreatePixelShader( psBuffer->GetBufferPointer( ),
	psBuffer->GetBufferSize( ), 0, &mPixelShader );

	psBuffer->Release( );
   
	D3D11_SAMPLER_DESC colorMapDesc;
    ZeroMemory( &colorMapDesc, sizeof( colorMapDesc ) );
    colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    tDevice->CreateSamplerState( &colorMapDesc, &mSampler );

	D3DX11CreateShaderResourceViewFromFile( tDevice,
        L"content\\fonts\\gamefont.png", 0, 0, &mFontRV, 0 );

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory( &resourceData, sizeof( resourceData ) );

	//clockwise
	WORD indices[] =
	{
		0,  1,  3,  
		1,  2,  3
	};

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory( &indexDesc, sizeof( indexDesc ) );
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof( WORD ) * 6;
	indexDesc.CPUAccessFlags = 0;
	resourceData.pSysMem = indices;
    
	tDevice->CreateBuffer( &indexDesc, &resourceData, &mIndexBuffer );

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0} //used for padding
	};

	unsigned int totalLayoutElements = ARRAYSIZE( vertexDesc );

	tDevice->CreateInputLayout(vertexDesc, totalLayoutElements, 
								vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(),
								&mInputLayout);
	vsBuffer->Release( );  
}

bool TextManager::CompileD3DShader(LPCWSTR filePath, char* entry, char* shaderModel, ID3DBlob** buffer )
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3DCOMPILE_DEBUG;
	#endif

    ID3DBlob* errorBuffer = 0;
    HRESULT result;

    result = D3DX11CompileFromFile( filePath, 0, 0, entry, shaderModel,
        shaderFlags, 0, 0, buffer, &errorBuffer, 0 );
    if( FAILED( result ) )
    {
        if( errorBuffer != 0 )
        {
            OutputDebugStringA( ( char* )errorBuffer->GetBufferPointer( ) );
            errorBuffer->Release( );
        }

        return false;
    }

    if( errorBuffer != 0 )
        errorBuffer->Release( );

    return true;
}