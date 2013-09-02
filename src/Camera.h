#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <xnamath.h>

class Camera{
public:

    Camera();

    void update( float aspectRatio );

    inline const XMFLOAT4X4& getView();
    inline const XMFLOAT4X4& getProj();

    inline XMFLOAT4& getPosition();
    inline void modPitch( float p );
    inline void modYaw( float y );

    inline void moveForwardBack( float d );
    inline void moveLeftRight( float d );

protected:

    XMFLOAT4 mPosition;

    XMFLOAT4X4 mView; //View Matrix
	XMFLOAT4X4 mProj; //Projection Matrix

    float mPitch;
    float mYaw;

    XMFLOAT4 mMove;
};

const XMFLOAT4X4& Camera::getView(){return mView;}
const XMFLOAT4X4& Camera::getProj(){return mProj;}


inline XMFLOAT4& Camera::getPosition(){return mPosition;}
inline void Camera::modPitch( float p ){mPitch += p;}
inline void Camera::modYaw( float y ){mYaw += y;}

inline void Camera::moveForwardBack( float d ){mMove.z = d;}
inline void Camera::moveLeftRight( float d ){mMove.x = d;}

#endif