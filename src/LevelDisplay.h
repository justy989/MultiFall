#ifndef FLOOR_DISPLAY_H
#define FLOOR_DISPLAY_H

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>
#include <xnamath.h>

#include "Level.h"

class LevelDisplay{
public:

    //Constr/Destr

    LevelDisplay();
    ~LevelDisplay();

    //Initialize input layout and set the wall and floor textures
    bool init( ID3D11Device* device, ID3DX11EffectTechnique* technique );

    //Set the current textures used by the Level display
    bool setTextures( ID3D11Device* device, LPCWSTR floorTexturePath, float floorClip, LPCWSTR wallTexturePath, float wallClip );

    //Create the floor display mesh from a generated(hopefully) floor
    bool createMeshFromLevel( ID3D11Device* device, Level& level, float blockDimension, float heightInterval );

    //Clear the vertex buffers and index buffers we have generated
    void clear();

    //Draw the Level Mesh that we generated
    void draw( ID3D11DeviceContext* device, ID3DX11Effect* fx );

protected:

    bool createFloorMesh( ID3D11Device* device, Level& level, float blockDimension, float heightInterval  );
    bool createWallsMesh( ID3D11Device* device, Level& level, float blockDimension, float heightInterval  );

protected:

    //Input layout for the DungeonVertex
    ID3D11InputLayout* mInputLayout;
    
    //World Matrix Constant Buffer
    ID3D11Buffer* mWorldCB;	

    //The floor that the player walks on, at every height
    ID3D11Buffer* mFloorVB;
    ID3D11Buffer* mFloorIB;

    //Walls placed at Height differences
    ID3D11Buffer* mWallsVB;
    ID3D11Buffer* mWallsIB;

    //Ramp Walls have to be separate because they are annoying...
    ID3D11Buffer* mRampWallsVB;

    //Textures
	ID3D11ShaderResourceView* mFloorTexture;
	ID3D11ShaderResourceView* mWallTexture;

    //Used to clip the textures based on Level::Blocks tileID and wallIDs
    float mFloorClip;
    float mWallClip;

    int mBlockCount; //How many blocks are there that we needed to draw
    int mWallCount; //How many walls are there?
    int mRampCount; //How many ramps are there?
};

#endif  