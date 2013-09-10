#ifndef D3DUTILS_H
#define D3DUTILS_H

#include "Log.h"
#include "Utils.h"

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
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

#endif