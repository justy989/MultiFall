#ifndef ENVIRONMENT_DISPLAY_H
#define ENVIRONMENT_DISPLAY_H

#include "Environment.h"

#include <D3D11.h>
#include <windows.h>
#include <xnamath.h>

#include <d3dx11effect.h>

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

protected:

    ID3D11InputLayout* mInputLayout;

    ID3D11Buffer* mBlockVB;
    ID3D11Buffer* mBlockIB;

    ID3D11Buffer* mRoomVB;
    ID3D11Buffer* mRoomIB;
};

#endif