#ifndef WORLD_DISPLAY_H
#define WORLD_DISPLAY_H

#include "DungeonDisplay.h"

class World;

class WorldDisplay{
public:

    WorldDisplay();
    ~WorldDisplay();

    //Initialize the different displays
	bool init( ID3D11Device* device, ID3DX11EffectTechnique* tech );

    //Draw the world in it's current state
    void draw( ID3D11DeviceContext* device, World& world );

    //Clear the different displays
    void clear();

    //Accessors

    inline ID3DX11EffectTechnique* getTechnique();
    inline ID3DX11Effect* getFX();

    inline DungeonDisplay& getDungeonDisplay();

protected:

    ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

    DungeonDisplay mDungeonDisplay;
};

inline ID3DX11EffectTechnique* WorldDisplay::getTechnique(){return mTech;}
inline ID3DX11Effect* WorldDisplay::getFX(){return mFX;}

inline DungeonDisplay& WorldDisplay::getDungeonDisplay(){return mDungeonDisplay;}

#endif