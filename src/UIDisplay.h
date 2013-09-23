#ifndef UIDISPLAY_H
#define UIDISPLAY_H

#include "UIWindow.h"
#include "TextManager.h"

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>

#define MAX_UI_QUADS 128

#define UI_INDEX_COUNT MAX_UI_QUADS * 6
#define UI_VERTEX_COUNT MAX_UI_QUADS * 4

#define UI_Z 0.0505f
#define UI_ELEMS_Z 0.05025f

//Determine which Z coord to use based on whether we are currently building elements in a window or the window itself
#define UI_DETERMINE_Z mBuildingElements ? UI_ELEMS_Z : UI_Z

#define UI_BAR_TOP 0.0f
#define UI_BAR_BOTTOM 0.0625f

#define UI_CORNER_TOP 0.0626f
#define UI_CORNER_BOTTOM 0.125f

class UIDisplay{
public:

    UIDisplay();
    ~UIDisplay();

    //Init input layout, shaders, sampler, and texture
    bool init( ID3D11Device* device, LPCWSTR uiTexturePath, LPCWSTR uiShaderPath );
    
    //Setup VB for drawing a window
    void buildWindowVB( UIWindow& window, float aspectRatio );

    //Clear everything we've allocated
    void clear();

    //Draw window Text
    void drawWindowText( ID3D11DeviceContext* device, UIWindow& window, TextManager& tm );

    //Draw a given window
    void drawUI( ID3D11DeviceContext* device );

    //Accessor to set the border dimension
    inline void setBorderDimension( float b );

    //Accessor to set the bg color
    inline void setBGColor( XMFLOAT4 color );

protected:

    //Setup vertex buffers for the border of an element
    void buildBorderVB( UIElement* elem, float aspectRatio, XMFLOAT2 offset = XMFLOAT2(0.0f, 0.0f) );

    //Setup verticies for the current tab, show others as well
    void buildTabVB( UIWindow& window, float aspectRatio );

    //Setup the background for the border of an element
    void buildBGVB(  UIElement* elem, XMFLOAT4& color );

    //Builds the various elements

    void buildTopBarVB( float start, float end, float top, float aspectRatio );
    void buildBottomBarVB( float start, float end, float top, float aspectRatio );
    void buildLeftBarVB( float start, float end, float left, float aspectRatio );
    void buildRightBarVB( float start, float end, float left, float aspectRatio );
    void buildCornerVB( float x, float y, float aspectRatio );

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

	ID3D11InputLayout* mInputLayout;

    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;

    ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTechnique;

    ID3D11ShaderResourceView* mTexture;

    FontVertex mVerts[ UI_VERTEX_COUNT ];
    int mVertsGenerated;

    float mBorderDimension;

    XMFLOAT4 mBGColor;

    bool mBuildingElements;
};

inline void UIDisplay::setBorderDimension( float b ){mBorderDimension = b;}
inline void UIDisplay::setBGColor( XMFLOAT4 color ){mBGColor = color;}

#endif