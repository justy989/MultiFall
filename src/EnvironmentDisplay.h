#ifndef ENVIRONMENT_DISPLAY_H
#define ENVIRONMENT_DISPLAY_H

#include "Environment.h"

#include <D3D11.h>
#include <windows.h>
#include <xnamath.h>

#include <d3dx11effect.h>

#define ROOM_INDEX_COUNT (4 * 6 * 4)

struct EnvVertex{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

class EnvironmentDisplay{
public:

    EnvironmentDisplay();

    bool init( ID3D11Device* device, ID3DX11EffectTechnique* tech );
    void clear();

    void draw( ID3D11DeviceContext* device, Environment& env, ID3DX11Effect* fx, ID3DX11EffectTechnique* tech );

    bool createRoomMesh( ID3D11Device* device, Environment::Room& room );

    inline void setBlockDimension( float b );
    inline void setHeightInterval( float h );
    inline void setDoorHeight( float d );

protected:

    bool createFrameMesh( ID3D11Device* device, Environment::Room& room );
    bool createFloorMesh( ID3D11Device* device, Environment::Room& room );
    bool createWallsMesh( ID3D11Device* device, Environment::Room& room );

protected:

    ID3D11InputLayout* mInputLayout;

    //The frame of the room, including doors
    ID3D11Buffer* mFrameVB;
    ID3D11Buffer* mFrameIB;

    //The floor that the player walks on, at every height
    ID3D11Buffer* mFloorVB;
    ID3D11Buffer* mFloorIB;

    //Walls placed at Height differences
    ID3D11Buffer* mWallsVB;
    ID3D11Buffer* mWallsIB;

    //Starts between floors
    ID3D11Buffer* mStairsVB;
    ID3D11Buffer* mStairsIB;

    //Water
    ID3D11Buffer* mWaterVB;
    ID3D11Buffer* mWaterIB;

    int mRoomSize;
    int mWallIndices;

    float mHeightInterval;
    float mBlockDimension;
    float mDoorHeight;
};

inline void EnvironmentDisplay::setHeightInterval( float h ){mHeightInterval = h;}
inline void EnvironmentDisplay::setDoorHeight( float d ){mDoorHeight = d;}
inline void EnvironmentDisplay::setBlockDimension( float b ){mBlockDimension = b;}

#endif