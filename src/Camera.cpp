#include "Camera.h"

Camera::Camera() {}

void Camera::update( float aspectRatio ){
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*3.14159f, aspectRatio, 0.1f, 10.0f);
	XMStoreFloat4x4(&mProj, P);

    static float angle = 0.0f;

    float x = cos( angle ) * 3.0f;
    float y = sin( angle ) * 3.0f;

    angle += 0.01f;

    XMVECTOR pos    = XMVectorSet(x + 0.8f, 3.0f, y + 0.8f, 1.0f);
	XMVECTOR target = XMVectorSet(0.8f, 0.0f, 0.8f, 1.0f);
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

    //XMMATRIX W = XMMatrixIdentity();
    //XMStoreFloat4x4(&mWorld, W);
}