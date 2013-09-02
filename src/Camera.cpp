#include "Camera.h"

Camera::Camera() {}

void Camera::update( float aspectRatio ){
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*3.14159f, aspectRatio, 0.1f, 10.0f);
	XMStoreFloat4x4(&mProj, P);

    XMVECTOR pos    = XMVectorSet(5.0f, 5.0f, 5.0f, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

    //XMMATRIX W = XMMatrixIdentity();
    //XMStoreFloat4x4(&mWorld, W);
}