#ifndef ENVIRONMENT_DISPLAY_H
#define ENVIRONMENT_DISPLAY_H

#include "Environment.h"

#include <D3D11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <windows.h>
#include <xnamath.h>

#include <d3dx11effect.h>

#define ROOM_INDEX_COUNT ( (4 * 6 * 4) + 6 )

struct EnvVertex{
    XMFLOAT3 position;
    XMFLOAT4 color;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};

class EnvironmentDisplay{
public:

    EnvironmentDisplay();

    //Init Environment Display by setting the input layout
    bool init( ID3D11Device* device, ID3DX11EffectTechnique* tech );

    //Clear all vertex and index buffers
    void clear();

    //Draw the last room mesh that was created
    void draw( ID3D11DeviceContext* device, Environment& env, ID3DX11Effect* fx, ID3DX11EffectTechnique* tech );

    //Create and save a mesh from a room
    bool createRoomMesh( ID3D11Device* device, Environment::Room& room );

    //Accessors

    inline void setBlockDimension( float b );
    inline void setHeightInterval( float h );
    inline void setDoorHeight( float d );

protected:

    bool createFrameMesh( ID3D11Device* device, Environment::Room& room );
    bool createFloorMesh( ID3D11Device* device, Environment::Room& room );
    bool createWallsMesh( ID3D11Device* device, Environment::Room& room );
	void createSurfaceNormal(EnvVertex* a, EnvVertex* b, EnvVertex* c); 

protected:

    ID3D11InputLayout* mInputLayout;

	//The floor textures and sampler to be passed to the shader
	ID3D11ShaderResourceView* mStoneFloor1RV;
	ID3D11ShaderResourceView* mStoneWall1RV;
	ID3D11SamplerState* mSampler;

    //The frame of the room, including doors
    ID3D11Buffer* mFrameVB;
    ID3D11Buffer* mFrameIB;

    //The floor that the player walks on, at every height
    ID3D11Buffer* mFloorVB;
    ID3D11Buffer* mFloorIB;

    //Walls placed at Height differences
    ID3D11Buffer* mWallsVB;
    ID3D11Buffer* mWallsIB;

    ID3D11Buffer* mRampWallsVB;

    //Starts between floors
    ID3D11Buffer* mStairsVB;
    ID3D11Buffer* mStairsIB;

    //Water
    ID3D11Buffer* mWaterVB;
    ID3D11Buffer* mWaterIB;

    int mRoomSize; //Size of the room in Blocks
    int mWallIndices; //How many wall indices need to be drawn
    int mRampCount; //How many ramps are there?

    float mHeightInterval; //Height between levels
    float mBlockDimension; //Block width and height
    float mDoorHeight; //Door height
    float mDoorWidth;
};

inline void EnvironmentDisplay::setHeightInterval( float h ){mHeightInterval = h;}
inline void EnvironmentDisplay::setDoorHeight( float d ){mDoorHeight = d;}
inline void EnvironmentDisplay::setBlockDimension( float b ){mBlockDimension = b;}

#endif