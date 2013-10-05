#ifndef POPULATION_DISPLAY_H
#define POPULATION_DISPLAY_H

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>
#include <xnamath.h>

#include "World.h"

struct CharacterBillboard{
    XMFLOAT4 pos; //Position
    XMFLOAT4 dimensions; //World Dimensions, and Texture dimensions
    //XMFLOAT2 texClipDimensions; //Texture Clipping dimensions
    //int frameX; //Frame X of animation
    //int frameY; //Frame Y of animation
};

class PopulationDisplay{
public:

    //Constr/Destr
    PopulationDisplay();
    ~PopulationDisplay();

    //Initialize input layout and set the wall and floor textures
    bool init( ID3D11Device* device, ID3DX11EffectTechnique* technique );

    //Clear the vertex buffers and index buffers we have generated
    void clear();

    //Draw the Level Mesh that we generated
    void draw( ID3D11DeviceContext* device );

    //Update billboard Instance list
    void updateBillboards( ID3D11DeviceContext* device, World& world, XMFLOAT4& CameraPos );

    inline void setDrawRange( float range );

protected:
    ID3D11InputLayout* mInputLayout; //Input Layout

    int mBillboardCount;
    CharacterBillboard mBillboards[ POPULATION_MAX_CHARACTERS ];

	ID3D11Buffer* mBillboardBuffer;
	ID3D11Buffer* mInstanceDataCB;

    ID3D11ShaderResourceView* mTextures[ 1 ];

    float mDrawRange;
};

inline void PopulationDisplay::setDrawRange( float range ){mDrawRange = range;}

#endif