#ifndef WORLD_DISPLAY_H
#define WORLD_DISPLAY_H

#include "EnvironmentDisplay.h"

class World;

class WorldDisplay{
public:

    WorldDisplay();
    ~WorldDisplay();

    //Initialize the different displays
    bool init( ID3D11Device* device );

    //Draw the world in it's current state
    void draw( ID3D11DeviceContext* device, World& world );

    //Clear the different displays
    void clear();

    //Accessors

    inline ID3DX11EffectTechnique* getTechnique();
    inline ID3DX11Effect* getFX();

    inline EnvironmentDisplay& getEnvDis();

protected:

    ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

    EnvironmentDisplay mEnvDisplay;
};

inline ID3DX11EffectTechnique* WorldDisplay::getTechnique(){return mTech;}
inline ID3DX11Effect* WorldDisplay::getFX(){return mFX;}

inline EnvironmentDisplay& WorldDisplay::getEnvDis(){return mEnvDisplay;}

#endif