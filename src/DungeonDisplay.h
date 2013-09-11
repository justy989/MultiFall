#ifndef DUNGEON_DISPLAY_H
#define DUNGEON_DISPLAY_H

#include "Dungeon.h"
#include "LevelDisplay.h"

class DungeonDisplay{
public:

    DungeonDisplay();

    //Init Dungeon Display by setting the input layout
    bool init( ID3D11Device* device, ID3DX11EffectTechnique* tech );

    //Clear all vertex and index buffers
    void clear();

    //Draw the last room mesh that was created
    void draw( ID3D11DeviceContext* device, ID3DX11EffectTechnique* tech );

	void drawLights( ID3D11DeviceContext* device, XMMATRIX* ViewProj);

    //Create the level Mesh
    bool createMeshFromLevel( ID3D11Device* device, Level& level );

    //Accessors

    inline void setBlockDimension( float b );
    inline void setHeightInterval( float h );

protected:

    //Input layout for the DungeonVertex
    ID3D11InputLayout* mInputLayout;

    //Texture sampler
	ID3D11SamplerState* mTextureSampler;

    //Level display
    LevelDisplay mLevelDisplay;
    
    float mHeightInterval; //Height between levels
    float mBlockDimension; //Block width and height
};

inline void DungeonDisplay::setHeightInterval( float h ){mHeightInterval = h;}
inline void DungeonDisplay::setBlockDimension( float b ){mBlockDimension = b;}

#endif