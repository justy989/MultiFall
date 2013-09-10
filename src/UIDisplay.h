#ifndef UIDISPLAY_H
#define UIDISPLAY_H

#include "UIWindow.h"
#include "TextManager.h"

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>

#define WINDOW_TITLE_LEFT 0
#define WINDOW_TITLE_MIDDLE 4
#define WINDOW_TITLE_RIGHT 8

#define WINDOW_BODY_TOP_LEFT 12
#define WINDOW_BODY_TOP_MIDDLE 16
#define WINDOW_BODY_TOP_RIGHT 20

#define WINDOW_BODY_MIDDLE_LEFT 24
#define WINDOW_BODY_MIDDLE_MIDDLE 28
#define WINDOW_BODY_MIDDLE_RIGHT 32

#define WINDOW_BODY_BOTTOM_LEFT 36
#define WINDOW_BODY_BOTTOM_MIDDLE 40
#define WINDOW_BODY_BOTTOM_RIGHT 44

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