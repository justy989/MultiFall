#ifndef UIDISPLAY_H
#define UIDISPLAY_H

#include "UIWindow.h"
#include "TextManager.h"

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>

#define UI_QUAD_COUNT 9

#define UI_VERTEX_COUNT UI_QUAD_COUNT * 4
#define UI_INDEX_COUNT UI_QUAD_COUNT * 6

class UIDisplay{
public:

    UIDisplay();
    ~UIDisplay();

    //Init input layout, shaders, sampler, and texture
    bool init( ID3D11Device* device, LPCWSTR uiTexturePath, LPCWSTR uiShaderPath );

    //Set input layout, shaders and sampler for UI and text
    void prepareUIRendering( ID3D11DeviceContext* device );
    
    //Clear everything we've allocated
    void clear();

    //Draw a given window
    void drawWindow( ID3D11DeviceContext* device, UIWindow& window, TextManager& tm );

protected:

    enum ElementIndexOffset{
        BG = 0,
        Border_Top = 6,
        Border_Bottom = 12,
        Border_Left = 18,
        Border_Right = 24,
        Border_Corner_Top_Left = 30,
        Border_Corner_Bottom_Left = 36,
        Border_Corner_Top_Right = 42,
        Border_Corner_Bottom_Right = 48,
    };

    ID3D11SamplerState* mSampler;

	ID3D11InputLayout* mInputLayout;

    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;

    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;

    ID3D11Buffer* mWorldCB;

    ID3D11ShaderResourceView* mTexture;
};

#endif