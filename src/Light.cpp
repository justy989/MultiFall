#include "Light.h"

PointLight::PointLight() :
    mRadius(1.5f),
    mIntensity(1.0f)
{
	mPosition = XMFLOAT3(0,0,0);
    mColor = XMFLOAT3(1,1,1);
}

void PointLight::update( float dt )
{
    //What should we do here?!
}

void PointLight::set( const XMFLOAT3& position, float radius, float intensity, const XMFLOAT3& color  )
{
    mPosition = position;
    mColor = color;
    mRadius = radius;
    mIntensity = intensity;
}