#ifndef LIGHT_H
#define LIGHT_H

#include "Utils.h"

class PointLight{
public:
	PointLight();

    void update( float dt );

    //Accessors

    void set( const XMFLOAT3& position, float radius, float intensity, const XMFLOAT3& color );

    inline float getRadius();
    inline float getIntensity();
    
    inline const XMFLOAT3& getPosition();
    inline const XMFLOAT3& getColor();

protected:

	float mRadius;
	float mIntensity;

	XMFLOAT3 mPosition;	
    XMFLOAT3 mColor;
};

inline float PointLight::getRadius(){return mRadius;}
inline float PointLight::getIntensity(){return mIntensity;}
    
inline const XMFLOAT3& PointLight::getPosition(){return mPosition;}
inline const XMFLOAT3& PointLight::getColor(){return mColor;}

#endif