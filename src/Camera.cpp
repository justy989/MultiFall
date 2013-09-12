#include "Camera.h"

Camera::Camera() :
    mPitch(0.0f),
    mYaw(0.0f),
    mFOV(50.0f)
{
    mPosition.x = 0.0f;
    mPosition.y = 2.0f;
    mPosition.z = 0.0f;
    mPosition.w = 0.0f;

    mMove.x = 0.0f;
    mMove.y = 0.0f;
    mMove.z = 0.0f;
    mMove.w = 1.0f;
}

void Camera::update( float aspectRatio ){

    float radianFOV = (mFOV / 180.0f) * 3.14159f;

    XMMATRIX P = XMMatrixPerspectiveFovLH(radianFOV, aspectRatio, 0.05f, 50.0f);
	XMStoreFloat4x4(&mProj, P);

    XMVECTOR defaultForward = XMVectorSet( 1.0f, 0.0f, 0.0f, 1.0f );

    XMVECTOR position = XMLoadFloat4( &mPosition );
    XMVECTOR move = XMLoadFloat4( &mMove );

    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, mPitch, mYaw);
    XMVECTOR camTarget = XMVector3TransformCoord(defaultForward, camRotationMatrix );
    XMVECTOR camMove = XMVector3TransformCoord(move, camRotationMatrix);
	XMVECTOR camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f );

    camTarget = XMVector3Normalize(camTarget);

	position += camMove;

	mMove.x = 0.0f;
    mMove.y = 0.0f;
    mMove.z = 0.0f;
    mMove.w = 1.0f;

	camTarget = position + camTarget;
    
	XMMATRIX V = XMMatrixLookAtLH( position, camTarget, camUp );
    XMStoreFloat4x4(&mView, V);

    XMStoreFloat4( &mPosition, position );
}