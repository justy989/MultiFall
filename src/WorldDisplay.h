#ifndef WORLD_DISPLAY_H
#define WORLD_DISPLAY_H

#include "LevelDisplay.h"
#include "LightDisplay.h"
#include "PopulationDisplay.h"

class World;

class WorldDisplay{
public:

    WorldDisplay();
    ~WorldDisplay();

    //Initialize the different displays
	bool init( ID3D11Device* device, ID3DX11EffectTechnique* tech );

    //Draw the world in it's current state
    void draw( ID3D11DeviceContext* device, ID3DX11Effect* fx, World& world, XMFLOAT4& cameraPos, float blockDimension );

    //Draw the point lights in a scene as sphere for deferred rendering
	void drawPointLights( ID3D11DeviceContext* device, ID3DX11Effect* fx,
                          XMFLOAT4& cameraPos, World& world, float blockDimension );

    //Clear the different displays
    void clear();

    //Accessors
    inline LevelDisplay& getLevelDisplay();
    inline LightDisplay& getLightDisplay();
    inline PopulationDisplay& getPopulationDisplay();

protected:

    //LevelDisplay: Draws Level, and Furniture
    LevelDisplay mLevelDisplay;

    //LightDisplay: Draw Lights from the world
    LightDisplay mLightDisplay;

    //PopulationDisplay: Draws Characters
    PopulationDisplay mPopDisplay;

    //ProjectileDisplay: Draws Projectiles

    //Interactive Object Display: Draws Interactive Objects
};

inline LevelDisplay& WorldDisplay::getLevelDisplay(){return mLevelDisplay;}
inline LightDisplay& WorldDisplay::getLightDisplay(){return mLightDisplay;}
inline PopulationDisplay& WorldDisplay::getPopulationDisplay(){return mPopDisplay;}
#endif