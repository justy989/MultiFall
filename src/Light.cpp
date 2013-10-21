#include "Light.h"

PointLight::PointLight() :
    mRadius(1.5f),
    mIntensity(1.0f)
{
    mColor = XMFLOAT3(1,1,1);
}

void PointLight::update( float dt )
{
    //What should we do here?! Flicker bro
}

void PointLight::set( float radius, float intensity, const XMFLOAT3& color  )
{
    mColor = color;
    mRadius = radius;
    mIntensity = intensity;
}