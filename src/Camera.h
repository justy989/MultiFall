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

protected:

    XMFLOAT4 mPosition;

    XMFLOAT4X4 mView; //View Matrix
	XMFLOAT4X4 mProj; //Projection Matrix
};

const XMFLOAT4X4& Camera::getView(){return mView;}
const XMFLOAT4X4& Camera::getProj(){return mProj;}

#endif