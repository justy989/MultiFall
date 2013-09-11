#include "LightManager.h"
#include "Log.h"

LightManager::LightManager()
{
	mNumDirLights = mNumPointLights = 0;
	mDirLights = new Light[MAXDIRLIGHTS];
	mPointLights = new Light[MAXPOINTLIGHTS];
}

LightManager::~LightManager()
{
	delete mPointLights;
	delete mDirLights;
}

void LightManager::addPointLight(XMFLOAT3 pos, float radius)
{
	if(mNumPointLights < MAXPOINTLIGHTS)
	{
		mPointLights[mNumPointLights].position = pos;
		mPointLights[mNumPointLights].radius = radius;

		mNumPointLights++;
	}
}

bool LightManager::init(ID3D11Device* device)
{
	//load mesh for point lights
	//the texture doesn't matter
	if( !mSphere.loadFromObj(device, "content/meshes/sphere.obj", L"content/textures/torch_texture.png"))
	{
		return false;
	}

	//shader variables to be updated
	D3D11_BUFFER_DESC constDesc;
    ZeroMemory( &constDesc, sizeof( constDesc ) );
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof( XMMATRIX );
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mWorldCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for Lighta" << LOG_ENDL;
        return false;
    }

	constDesc.ByteWidth = sizeof( PS_CB );
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mPSCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for Lights" << LOG_ENDL;
        return false;
    }

	return true;
}

void LightManager::DrawLights(ID3D11DeviceContext* device, XMMATRIX* ViewProj)
{
	XMMATRIX invviewproj = XMMatrixInverse(&XMMatrixDeterminant(*ViewProj), *ViewProj);
	invviewproj = XMMatrixTranspose(invviewproj);

	struct PS_CB pscb;
	pscb.invViewProj = invviewproj;

	for(int i = 0; i < mNumPointLights; i++)
	{
		XMMATRIX worldviewproj =	XMMatrixScaling(mPointLights[i].radius, mPointLights[i].radius, mPointLights[i].radius) *
									XMMatrixTranslation(mPointLights[i].position.x, mPointLights[i].position.y, mPointLights[i].position.z);
		worldviewproj *= *ViewProj;

		worldviewproj = XMMatrixTranspose( worldviewproj );

		device->UpdateSubresource( mWorldCB, 0, 0, &worldviewproj, 0, 0 );
		device->VSSetConstantBuffers( 0, 1, &mWorldCB );

		pscb.lightPosition = XMFLOAT4(mPointLights[i].position.x,mPointLights[i].position.y,mPointLights[i].position.z,1);
		pscb.lightRadIntensity = XMFLOAT4(mPointLights[i].radius, mPointLights[i].intensity, 0, 0);

		device->UpdateSubresource( mPSCB, 0, 0, &pscb, 0, 0 );
		device->PSSetConstantBuffers( 1, 1, &mPSCB );

		mSphere.draw(device);
	}
}