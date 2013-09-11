#ifndef UIDISPLAY_H
#define UIDISPLAY_H

#include "UIWindow.h"
#include "TextManager.h"

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>

#define MAX_UI_QUADS 128

#define UI_INDEX_COUNT MAX_UI_QUADS * 6
#define UI_VERTEX_COUNT MAX_UI_QUADS * 4

class UIDisplay{
public:

    UIDisplay();
    ~UIDisplay();

    //Init input layout, shaders, sampler, and texture
    bool init( ID3D11Device* device, LPCWSTR uiTexturePath, LPCWSTR uiShaderPath );

    //Set input layout, shaders and sampler for UI and text
    void prepareUIRendering( ID3D11DeviceContext* device );
    
    //Setup vertex buffers for the next frame
    void prepareWindow( ID3D11DeviceContext* device, UIWindow& window, TextManager& tm );

    //Clear everything we've allocated
    void clear();

    //Draw a given window
    void drawUI( ID3D11DeviceContext* device );

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

    FontVertex mVerts[ UI_VERTEX_COUNT ];
    int mVertsGenerated;
};

#endif