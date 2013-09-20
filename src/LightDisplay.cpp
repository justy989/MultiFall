#include "LightDisplay.h"
#include "Log.h"

LightDisplay::LightDisplay() :
    mWorldCB(NULL),
    mPointLightCB(NULL),
    mInsideLightRS(NULL),
    mOutsideLightRS(NULL)
{

}

LightDisplay::~LightDisplay()
{

}

bool LightDisplay::init(ID3D11Device* device)
{
	//load mesh for point lights
	//the texture doesn't matter
	if( !mSphere.loadFromObj(device, "content/meshes/sphere.obj", L"content/textures/torch_texture.png")){
		return false;
	}

    //shader variables to be updated
	D3D11_BUFFER_DESC constDesc;
    ZeroMemory( &constDesc, sizeof( constDesc ) );
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof( XMMATRIX );
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mWorldCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for Light" << LOG_ENDL;
        return false;
    }

    constDesc.ByteWidth = sizeof( PointLightCBLayout );
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mPointLightCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for Lights" << LOG_ENDL;
        return false;
    }

	D3D11_RASTERIZER_DESC rasterDesc;

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

	if( FAILED( device->CreateRasterizerState(&rasterDesc, &mOutsideLightRS) ) ){
        LOG_ERRO << "Failed to create Rasterizer State for Lights" << LOG_ENDL;
        return false;
    }

	rasterDesc.CullMode = D3D11_CULL_FRONT;

	if( FAILED( device->CreateRasterizerState(&rasterDesc, &mInsideLightRS) ) ){
        LOG_ERRO << "Failed to create Rasterizer State for Lights" << LOG_ENDL;
        return false;
    }

	return true;
}

void LightDisplay::clear()
{
    ReleaseCOM( mWorldCB );
    ReleaseCOM( mPointLightCB );

    ReleaseCOM( mInsideLightRS );
    ReleaseCOM( mOutsideLightRS );

    mSphere.clear();
}

void LightDisplay::drawPointLights( ID3D11DeviceContext* device, ID3DX11Effect* fx,
                                    World& world, XMFLOAT4& cameraPos )
{
    //Save the rasterizer state
	ID3D11RasterizerState* prevRast;
	device->RSGetState(&prevRast);

    Level& level = world.getLevel();
    PointLightCBLayout plCB;

    for(ushort i = 0; i < level.getNumLights(); i++)
	{
        //Update the world Matrix Constant Buffer
        XMMATRIX world = XMMatrixScaling( level.getLight(i).getRadius(), level.getLight(i).getRadius(), level.getLight(i).getRadius() ) *
                         XMMatrixTranslation( level.getLight(i).getPosition().x, level.getLight(i).getPosition().y, level.getLight(i).getPosition().z );

		world = XMMatrixTranspose( world );

        device->UpdateSubresource( mWorldCB, 0, 0, &world, 0, 0 );
		device->VSSetConstantBuffers( 1, 1, &mWorldCB );

        //Update the Light Constant Buffer
		XMFLOAT4 lightPosition = XMFLOAT4( level.getLight(i).getPosition().x, level.getLight(i).getPosition().y, level.getLight(i).getPosition().z, 1);
		XMFLOAT4 lightRadIntensity = XMFLOAT4( level.getLight(i).getRadius(), level.getLight(i).getIntensity(), 0, 0);

        plCB.position = lightPosition;
        plCB.radIntensity = lightRadIntensity;

        device->UpdateSubresource( mPointLightCB, 0, 0, &plCB, 0, 0 );
		device->PSSetConstantBuffers( 2, 1, &mPointLightCB );

        //Calculate distance of the camera to the light
		XMFLOAT3 v1 = XMFLOAT3( cameraPos.x, cameraPos.y, cameraPos.z);
		XMFLOAT3 v2 = XMFLOAT3( lightPosition.x, lightPosition.y, lightPosition.z );

		XMVECTOR vector1 = XMLoadFloat3(&v1);
		XMVECTOR vector2 = XMLoadFloat3(&v2);
		XMVECTOR vectorSub = XMVectorSubtract(vector1,vector2);
		XMVECTOR length = XMVector3LengthEst(vectorSub);

		float distance = 0.0f;
		XMStoreFloat(&distance, length);

        //Set render state based on whether we are inside the light or not
		if(distance < lightRadIntensity.x + 0.051f)
		{
			device->RSSetState(mInsideLightRS);
		}
		else
		{
			device->RSSetState(mOutsideLightRS);
		}

        //Draw the sphere bro
		mSphere.draw(device);
	}

    //Reset the rasterizer state
	device->RSSetState(prevRast);
}