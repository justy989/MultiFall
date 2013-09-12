#ifndef D3DUTILS_H
#define D3DUTILS_H

#include "Log.h"
#include "Utils.h"

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>
#include <D3Dcompiler.h>

bool CompileD3DShader(LPCWSTR filePath, char* entry, char* shaderModel, ID3DBlob** buffer )
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
            LOG_ERRO << (char*)errorBuffer->GetBufferPointer() << LOG_ENDL;
			ReleaseCOM(errorBuffer);
        }

        return false;
    }

    if( errorBuffer != 0 )
        errorBuffer->Release( );

    return true;
}

bool compileEffect(ID3D11Device* device, LPCWSTR filepath, ID3DX11Effect** fx )
{
	//Compile shader
    DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(filepath, 0, 0, 0, "fx_5_0", shaderFlags, 
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if( compilationMsgs != 0 )
	{
		LOG_ERRO << (char*)compilationMsgs->GetBufferPointer() << LOG_ENDL;
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if(FAILED(hr))
	{
        LOG_ERRO << "D3DCompileFromFile() failed" << LOG_ENDL;
        return false;
	}

	if(FAILED( D3DX11CreateEffectFromMemory( compiledShader->GetBufferPointer(), 
                                             compiledShader->GetBufferSize(), 
                                             0,  device, fx) ) ){
        LOG_ERRO << "CreateEffectFromMemory() Failed" << LOG_ENDL;
        return false;
    }

    ReleaseCOM( compiledShader );

    return true;
}

#endif