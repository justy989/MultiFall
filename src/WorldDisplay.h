#ifndef WORLD_DISPLAY_H
#define WORLD_DISPLAY_H

#include "EnvironmentDisplay.h"

class World;

class WorldDisplay{
public:

    WorldDisplay();
    ~WorldDisplay();

    bool init( ID3D11Device* device );

    void draw( ID3D11DeviceContext* device, World& world );

    void clear();

    inline ID3DX11EffectTechnique* getTechnique();
    inline ID3DX11Effect* getFX();

protected:

    ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

    EnvironmentDisplay mEnvDisplay;
};

inline ID3DX11EffectTechnique* WorldDisplay::getTechnique(){return mTech;}
inline ID3DX11Effect* WorldDisplay::getFX(){return mFX;}

#endif