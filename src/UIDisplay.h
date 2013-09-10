#ifndef UIDISPLAY_H
#define UIDISPLAY_H

#include "UIWindow.h"
#include "TextManager.h"

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>

#define WINDOW_TITLE_LEFT 0
#define WINDOW_TITLE_MIDDLE 6
#define WINDOW_TITLE_RIGHT 12

#define WINDOW_BODY_TOP_LEFT 18
#define WINDOW_BODY_TOP_MIDDLE 24
#define WINDOW_BODY_TOP_RIGHT 30

#define WINDOW_BELOW_TAB_LEFT 36
#define WINDOW_BELOW_TAB_RIGHT 42

#define WINDOW_BODY_MIDDLE_LEFT 48
#define WINDOW_BODY_MIDDLE_MIDDLE 54
#define WINDOW_BODY_MIDDLE_RIGHT 60

#define WINDOW_BODY_BOTTOM_LEFT 66
#define WINDOW_BODY_BOTTOM_MIDDLE 72
#define WINDOW_BODY_BOTTOM_RIGHT 78

#define WINDOW_CHECKBOX_UNCHECKED 84
#define WINDOW_CHECKBOX_CHECKED 90

#define WINDOW_SLIDER_LEFT_ARROW 96
#define WINDOW_SLIDER_RIGHT_ARROW 102
#define WINDOW_SLIDER_FILLED_BAR 108
#define WINDOW_SLIDER_EMPTY_BAR 114

#define UI_INDEX_COUNT 20 * 6

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