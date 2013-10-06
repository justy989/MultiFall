#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <xnamath.h>

class Camera{
public:

    Camera();

    //Update the matricies based on changes in camera position and rotation
    void update( float aspectRatio );

    //Change the pitch
    inline void modPitch( float p );

    //Change the yaw
    inline void modYaw( float y );

    //Move forward or back based on whether d is positive or negative
    inline void moveForwardBack( float d );

    //Move left or right based on whether d is positive or negative
    inline void moveLeftRight( float d );

    //Accessors

    inline const XMFLOAT4X4& getView();
    inline const XMFLOAT4X4& getProj();

    inline XMFLOAT4& getPosition();

    //Degrees
    inline void setFOV( float f );
    inline float getFOV();

    inline void setNear( float f );
    inline void setFar( float f );

    inline float getPitch();
    inline float getYaw();

    inline float getNear( );
    inline float getFar( );

protected:

    XMFLOAT4 mPosition;

    XMFLOAT4X4 mView; //View Matrix
	XMFLOAT4X4 mProj; //Projection Matrix

    float mPitch;
    float mYaw;

    float mNearPlane;
    float mFarPlane;

    float mFOV; //In degrees

    XMFLOAT4 mMove;
};

const XMFLOAT4X4& Camera::getView(){return mView;}
const XMFLOAT4X4& Camera::getProj(){return mProj;}


inline XMFLOAT4& Camera::getPosition(){return mPosition;}
inline void Camera::modPitch( float p ){mPitch += p;}
inline void Camera::modYaw( float y ){mYaw += y;}

inline void Camera::moveForwardBack( float d ){mMove.z = d;}
inline void Camera::moveLeftRight( float d ){mMove.x = d;}

inline void Camera::setFOV( float f ){mFOV = f;}
inline float Camera::getFOV(){return mFOV;}

inline float Camera::getPitch(){return mPitch;}
inline float Camera::getYaw(){return mYaw;}

inline void Camera::setNear( float f ){mFarPlane = f;}
inline void Camera::setFar( float f ){mNearPlane = f;}

inline float Camera::getNear(){return mNearPlane;}
inline float Camera::getFar(){return mFarPlane;}

#endif