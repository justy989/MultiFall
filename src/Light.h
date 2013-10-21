#ifndef LIGHT_H
#define LIGHT_H

#include "Utils.h"

//Class describes a unique point light
class PointLight{
public:
	PointLight();

    void update( float dt );

    //Accessors

    void set( float radius, float intensity, const XMFLOAT3& color );

    inline float getRadius();
    inline float getIntensity();
   
    inline XMFLOAT3& getColor();

protected:

	float mRadius;
	float mIntensity;

    XMFLOAT3 mColor;
};

inline float PointLight::getRadius(){return mRadius;}
inline float PointLight::getIntensity(){return mIntensity;}
    
inline XMFLOAT3& PointLight::getColor(){return mColor;}

#endif