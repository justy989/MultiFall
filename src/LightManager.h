#ifndef LIGHTMAN_H
#define LIGHTMAN_H

#define MAXDIRLIGHTS 1
#define MAXPOINTLIGHTS 250

#include "StaticMesh.h"
#include "Light.h"
#include "Camera.h"

class LightManager
{
public:
	LightManager();
	~LightManager();

	bool init(ID3D11Device* device);

	void DrawPointLights(ID3D11DeviceContext* device, XMMATRIX* ViewProj, XMFLOAT4* cameraPos);

	//probably going to add support for cubemaps later (whatever that means)
	void addDirectional(XMFLOAT3 dir);
	void addPointLight(XMFLOAT3 pos, float radius);

	struct PS_CB
	{
		XMMATRIX invViewProj;                              
		XMFLOAT4 lightPosition;
		XMFLOAT4 lightRadIntensity;
	} PS_CB;

protected:
	StaticMesh mSphere; //for point lights

	ID3D11RasterizerState* mInsideLight;
	ID3D11RasterizerState* mOutsideLight;

	ID3D11Buffer* mWorldCB;
	ID3D11Buffer* mPSCB;

	int mNumDirLights, mNumPointLights;
	Light* mDirLights;
	Light* mPointLights;
};

#endif