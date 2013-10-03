#ifndef LIGHTMAN_H
#define LIGHTMAN_H

#define MAXDIRLIGHTS 1
#define MAXPOINTLIGHTS 250

#include "StaticMesh.h"
#include "Light.h"
#include "World.h"

#include <d3dx11effect.h>

class LightDisplay
{
public:

	LightDisplay();
	~LightDisplay();

    //Initialize Constant buffers, rasterizer states, and Sphere
	bool init( ID3D11Device* device );

    //Clear everything!
    void clear();

    //Draw point lights in the world
	void drawPointLights( ID3D11DeviceContext* device, ID3DX11Effect* fx, 
                          World& world, XMFLOAT4& cameraPos );

protected:

    //Point Light Constant Buffer
    struct PointLightCBLayout
	{                             
		XMFLOAT4 position;
		XMFLOAT4 radIntensity;
	};

    //World Matrix Constant Buffer
    ID3D11Buffer* mWorldCB;	
    ID3D11Buffer* mPointLightCB;

    //Raster States for in and outside lights
    ID3D11RasterizerState* mInsideLightRS;
	ID3D11RasterizerState* mOutsideLightRS;

    //List of point lights to use based on light type
    PointLight mPointLights[ LEVEL_LIGHT_TYPE_COUNT - 1 ];

	StaticMesh mSphere; //for point lights
};

#endif