#include "WorldDisplay.h"

#include "Log.h"
#include "Utils.h"

#include <D3DX11.h>

WorldDisplay::WorldDisplay() : mFX(NULL),
    mTech(NULL)
{

}

WorldDisplay::~WorldDisplay()
{
    clear();
}

bool WorldDisplay::init( ID3D11Device* device )
{
    //Compile shader
    DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"content/shaders/color.fx", 0, 0, 0, "fx_5_0", shaderFlags, 
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

	if(FAILED( D3DX11CreateEffectFromMemory( compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 
        0, device, &mFX) ) ){
        LOG_ERRO << "CreateEffectFromMemory() Failed" << LOG_ENDL;
        return false;
    }

	// Done with compiled shader.
	ReleaseCOM(compiledShader);
	mTech = mFX->GetTechniqueByName("ColorTech"); //Temporary

    LOG_INFO << "Loaded color.fx shader Successfully" << LOG_ENDL;

    if( !mEnvDisplay.init( device, mTech ) ){
        return false;
    }

    return true;
}

void WorldDisplay::draw( ID3D11Device* device, World& world )
{
    //mEnvDisplay.draw( device, world.getEnvironment() );
}

void WorldDisplay::clear()
{
    mEnvDisplay.clear();

    ReleaseCOM( mFX );
}