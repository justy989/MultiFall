#ifndef ENVIRONMENT_DISPLAY_H
#define ENVIRONMENT_DISPLAY_H

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

protected:

    ID3D11InputLayout* mInputLayout;
};

#endif