#include "Camera.h"

Camera::Camera() :
    mPitch(0.0f),
    mYaw(0.0f)
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
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*3.14159f, aspectRatio, 0.1f, 10.0f);
	XMStoreFloat4x4(&mProj, P);

    XMVECTOR defaultForward = XMVectorSet( 1.0f, 0.0f, 0.0f, 1.0f );
    XMVECTOR defaultRight = XMVectorSet( 0.0f, 0.0f, -1.0f, 1.0f );

    XMVECTOR position = XMLoadFloat4( &mPosition );
    XMVECTOR move = XMLoadFloat4( &mMove );

    /*
    static float angle = 0.0f;

    float x = cos( angle ) * 3.0f;
    float y = sin( angle ) * 3.0f;

    angle += 0.01f;

    XMVECTOR pos    = XMVectorSet(x + 0.8f, 5.0f, y + 0.8f, 1.0f);
	XMVECTOR target = XMVectorSet(0.8f, 0.0f, 0.8f, 1.0f);
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);*/

    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, mPitch, mYaw);
    XMVECTOR camTarget = XMVector3TransformCoord(defaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);
	
	/*
	// First-Person Camera
	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);*/
	
	// Free-Look Camera
    XMVECTOR camMove = XMVector3TransformCoord(move, camRotationMatrix);
	XMVECTOR camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f );

	position += camMove;

	mMove.x = 0.0f;
    mMove.y = 0.0f;
    mMove.z = 0.0f;
    mMove.w = 1.0f;

	camTarget = position + camTarget;	
        
    XMMATRIX V = XMMatrixLookAtLH( position, camTarget, camUp );
    XMStoreFloat4x4(&mView, V);

    XMStoreFloat4( &mPosition, position );

    //XMMATRIX W = XMMatrixIdentity();
    //XMStoreFloat4x4(&mWorld, W);
}