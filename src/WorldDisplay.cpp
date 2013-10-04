#include "WorldDisplay.h"

#include "Log.h"
#include "Utils.h"

WorldDisplay::WorldDisplay()
{

}

WorldDisplay::~WorldDisplay()
{
    clear();
}

bool WorldDisplay::init( ID3D11Device* device, ID3DX11EffectTechnique* tech )
{
    //Initialize each display object

    if( !mLevelDisplay.init( device, tech ) ){
        return false;
    }

    if( !mLevelDisplay.setTextures( device, L"content/textures/stonefloorsheet1.png", 0.25f, 
                                            L"content/textures/stonewall1.png", 1.0f,
                                            L"content/textures/stonewall1.png", 1.0f ) ){
        return false;
    }

    if( !mLightDisplay.init( device ) ){
        return false;
    }

    return true;
}

void WorldDisplay::draw( ID3D11DeviceContext* device, ID3DX11Effect* fx, World& world )
{
    mLevelDisplay.draw( device, fx, world );
}

void WorldDisplay::drawPointLights( ID3D11DeviceContext* device, ID3DX11Effect* fx,
                                    XMFLOAT4& cameraPos, World& world )
{
    mLightDisplay.drawPointLights( device, fx, world, cameraPos );
}

void WorldDisplay::clear()
{
    mLevelDisplay.clear();
    mLightDisplay.clear();
}