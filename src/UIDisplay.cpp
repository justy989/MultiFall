#include "UIDisplay.h"
#include "Log.h"

#include "D3DUtils.h"

UIDisplay::UIDisplay() :
    mVB(NULL),
    mIB(NULL),
    mTexture(NULL),
    mInputLayout(NULL),
    mFX(NULL),
    mTechnique(NULL),
    mVertsGenerated(0),
    mBorderDimension(0.01f)
{
    mBGColor.x = 0.0f;
    mBGColor.y = 0.0f;
    mBGColor.z = 0.0f;
    mBGColor.w = 0.75f;
}

UIDisplay::~UIDisplay()
{
    
}

bool UIDisplay::init( ID3D11Device* device, LPCWSTR uiTexturePath, LPCWSTR uiShaderPath )
{
    if( FAILED( D3DX11CreateShaderResourceViewFromFile( device,
        uiTexturePath, 0, 0, &mTexture, 0 ) ) ){
        LOG_ERRO << "Failed to create UI Texture out of: " << LOG_WC_TO_C(uiTexturePath) << LOG_ENDL;
        return false;
    }

    if( !compileEffect( device, uiShaderPath, &mFX ) ){
        LOG_ERRO << "Unable to compile effect for UI: " << LOG_WC_TO_C(uiShaderPath) << LOG_ENDL;
        return false;
    }

    //Dont sweat the technique
    mTechnique = mFX->GetTechniqueByName("RenderUI");

    ushort inds[ UI_INDEX_COUNT ];
    ushort v = 0;

    for(int i = 0; i < UI_INDEX_COUNT; ){
        inds[i] = v; i++;
        inds[i] = v + 1; i++;
        inds[i] = v + 2; i++;

        inds[i] = v; i++;
        inds[i] = v + 2; i++;
        inds[i] = v + 3; i++;

        v += 4;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(ushort) * UI_INDEX_COUNT;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = inds;

    //Create IB
    if( FAILED(device->CreateBuffer(&ibd, &iinitData, &mIB) ) ){
        LOG_ERRO << "Unable to allocate Index Buffer for UI" << LOG_ENDL;
        return false;
    }

    D3DX11_PASS_DESC passDesc;
    HRESULT hr = mTechnique->GetPassByIndex(0)->GetDesc( &passDesc );

    //Input Description
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	unsigned int totalLayoutElements = ARRAYSIZE( vertexDesc );

	if( FAILED( device->CreateInputLayout(vertexDesc, totalLayoutElements, 
								passDesc.pIAInputSignature, 
		                        passDesc.IAInputSignatureSize,
								&mInputLayout) ) ){
        LOG_ERRO << "Failed to create Input Layout for UI" << LOG_ENDL;
        return false;
    }

    return true;
}

void UIDisplay::clear()
{
    ReleaseCOM( mVB );
    ReleaseCOM( mIB );
    ReleaseCOM( mTexture );
    ReleaseCOM( mFX );
    //ReleaseCOM( mPixelShader );
    //ReleaseCOM( mVertexShader );
    //ReleaseCOM( mSampler );
    ReleaseCOM( mInputLayout );

    LOG_INFO << "Released UI Buffers, textures, shaders, sampler, and input layout" << LOG_ENDL;
}

void UIDisplay::prepareUIRendering( ID3D11DeviceContext* device )
{
    //Setup input layout and topology
    //device->IASetInputLayout( mInputLayout );
    //device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    //Setup vertex and pixel shaders
	//device->VSSetShader( mVertexShader, 0, 0 );
	//device->PSSetShader( mPixelShader, 0, 0 );

    //Setup sampler
	//device->PSSetSamplers( 0, 1, &mSampler );

    //Setup blend state
    //float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    //uint sampleMask = 0xFFFFFFFF;

    //device->OMSetBlendState( mBlender, blendFactor, sampleMask );
}

void UIDisplay::buildWindowVB( UIWindow& window, float aspectRatio )
{
    //Build window border
    buildBorderVB( &window, aspectRatio );

    if( window.getTabCount() > 1 ){
        buildTabVB( window, aspectRatio );
    }else{
        //Build window Body by subtracting the title bar and the border
        UIWindow w;
        float borderWidth = mBorderDimension;
        float borderHeight = borderWidth * aspectRatio;

        w.setPosition( XMFLOAT2(window.getPosition().x + borderWidth, (window.getPosition().y + UIWINDOW_TITLE_BAR_HEIGHT) ) );
        w.setDimension( XMFLOAT2(window.getDimension().x - ( borderWidth * 2.0f ), (window.getDimension().y - UIWINDOW_TITLE_BAR_HEIGHT ) - borderHeight ) );
        buildBorderVB( &w, aspectRatio );
    }

    //Build BG
    buildBGVB( &window );

    //Loop through and build elements in the current tab
}

void UIDisplay::buildTabVB( UIWindow& window, float aspectRatio )
{
    
}

void UIDisplay::drawWindowText( ID3D11DeviceContext* device, UIWindow& window, TextManager& tm )
{
    UIWindow::Text* text;
    int textCount;
    window.getText( &text, &textCount );

    float x = 0.0f;
    float y = 0.0f;
    float len = static_cast<float>( strlen( text->message ) ) * FONTWIDTH;

    y = window.getPosition().y + (UIWINDOW_TITLE_BAR_HEIGHT / 2.0f) - ( FONTHEIGHT / 2.0f );
    x = window.getPosition().x + ( window.getDimension().x / 2.0f ) - ( len / 2.0f );

    //Convert to weird text manager cooords :D
    //x += 1.0f; x /= 2.0f;
    //y += 1.0f; y /= 2.0f;

    D3DX11_TECHNIQUE_DESC techDesc;
	mTechnique->GetDesc( &techDesc );

	for(ushort p = 0; p < techDesc.Passes; ++p)
	{
        mTechnique->GetPassByIndex(p)->Apply(0, device);
        tm.drawString( device, text->message, x, y );
    }
}


void UIDisplay::drawUI( ID3D11DeviceContext* device )
{
    //Setup the vertex buffer
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(FontVertex) * mVertsGenerated;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = mVerts;
    ID3D11Device* dev;
    device->GetDevice(&dev);

    if( FAILED(dev->CreateBuffer(&vbd, &vinitData, &mVB) ) ){
        LOG_ERRO << "Unable to allocate Vertex Buffer for UI" << LOG_ENDL;
        //return false;
    }

    //Setup temp vars
    uint stride = sizeof(FontVertex);
    uint offset = 0;

    device->IASetInputLayout( mInputLayout );
    device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    //Set vertex and index buffers
    device->IASetVertexBuffers( 0, 1, &mVB, &stride, &offset );
    device->IASetIndexBuffer( mIB, DXGI_FORMAT_R16_UINT, offset );

    ID3DX11EffectShaderResourceVariable* mfxTex = mFX->GetVariableByName("uiTexture")->AsShaderResource(); 
    mfxTex->SetResource(mTexture);

    XMMATRIX world = XMMatrixIdentity();

    ID3DX11EffectMatrixVariable* mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));

    int indices = ( mVertsGenerated / 4 ) * 6;

    D3DX11_TECHNIQUE_DESC techDesc;
	mTechnique->GetDesc( &techDesc );

	for(ushort p = 0; p < techDesc.Passes; ++p)
	{
        mTechnique->GetPassByIndex(p)->Apply(0, device);
        
        //Draw the verts we generated wooo
        device->DrawIndexed( indices, 0, 0 );
    }

    mVertsGenerated = 0;

    ReleaseCOM(mVB);
}

void UIDisplay::buildBorderVB( UIElement* elem, float aspectRatio )
{
    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    float l = elem->getPosition().x;
    float t = -elem->getPosition().y;
    float r = l + borderWidth;
    float b = t - borderHeight;

    //Generate each part of the window

    //TLC
    buildCornerVB( l, t, aspectRatio );

    //TB
    l = r;
    r = ( elem->getPosition().x + elem->getDimension().x ) - borderWidth;

    buildTopBarVB( l, r, t, aspectRatio );

    //TRC
    l = r;
    r += borderWidth;

    buildCornerVB( l, t, aspectRatio );

    //RB
    t = b;
    b = (-elem->getPosition().y - elem->getDimension().y) + borderHeight;

    buildRightBarVB( t, b, l, aspectRatio );

    //BRC
    t = b;
    b -= borderHeight;

    buildCornerVB( l, t, aspectRatio );

    //BB
    l = elem->getPosition().x + borderWidth;
    r = (elem->getPosition().x + elem->getDimension().x) - borderWidth;

    buildBottomBarVB( l, r, t, aspectRatio );

    //BRC
    l = elem->getPosition().x;
    r = l + borderWidth;

    buildCornerVB( l, t, aspectRatio );

    //LB
    t = -elem->getPosition().y - borderHeight;
    b = (-elem->getPosition().y - elem->getDimension().y) + borderHeight;

    buildLeftBarVB( t, b, l, aspectRatio );
}

void UIDisplay::buildTopBarVB( float start, float end, float top, float aspectRatio )
{
    FontVertex* v = mVerts + mVertsGenerated;
    XMFLOAT4 c = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    float l = start;
    float r = end;
    float t = top;
    float b = t - borderHeight;

    float wrap = ( r - l ) / ( borderWidth * 4.0f );

    v->position = XMFLOAT4( l, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( r, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( r, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( l, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_BOTTOM );
    v++;

    mVertsGenerated += 4;
}

void UIDisplay::buildBottomBarVB( float start, float end, float top, float aspectRatio )
{
    FontVertex* v = mVerts + mVertsGenerated;
    XMFLOAT4 c = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    float l = start;
    float r = end;
    float t = top;
    float b = t - borderHeight;

    float wrap = ( r - l ) / ( borderWidth * 4.0f );

    v->position = XMFLOAT4( l, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( r, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( r, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( l, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_TOP );
    v++;

    mVertsGenerated += 4;
}

void UIDisplay::buildLeftBarVB( float start, float end, float left, float aspectRatio )
{
    FontVertex* v = mVerts + mVertsGenerated;
    XMFLOAT4 c = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    float l = left;
    float r = l + borderWidth;
    float t = start;
    float b = end;

    float wrap = ( b - t ) / ( borderHeight * 4.0f );

    v->position = XMFLOAT4( l, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( r, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( r, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( l, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_TOP );
    v++;

    mVertsGenerated += 4;
}

void UIDisplay::buildRightBarVB( float start, float end, float left, float aspectRatio )
{
    FontVertex* v = mVerts + mVertsGenerated;
    XMFLOAT4 c = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    float l = left;
    float r = l + borderWidth;
    float t = start;
    float b = end;

    float wrap = ( b - t ) / ( borderHeight * 4.0f );

    v->position = XMFLOAT4( l, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( r, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( r, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( l, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_BOTTOM );
    v++;

    mVertsGenerated += 4;
}

void UIDisplay::buildCornerVB( float x, float y, float aspectRatio )
{
    FontVertex* v = mVerts + mVertsGenerated;
    XMFLOAT4 c = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    float l = x;
    float r = l + borderWidth;
    float t = y;
    float b = t - borderHeight;

    v->position = XMFLOAT4( l, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_CORNER_TOP );
    v++;

    v->position = XMFLOAT4( r, t, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, UI_CORNER_TOP );
    v++;

    v->position = XMFLOAT4( r, b , UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, UI_CORNER_BOTTOM );
    v++;

    v->position = XMFLOAT4( l, b, UI_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_CORNER_BOTTOM );
    v++;

    mVertsGenerated += 4;
}

void UIDisplay::buildBGVB( UIElement* elem )
{
    FontVertex* v = mVerts + mVertsGenerated;

    v->position = XMFLOAT4( elem->getPosition().x, -elem->getPosition().y, UI_Z, 1.0f );
    v->color = mBGColor;
    v->tex = XMFLOAT2( 0.0f, 0.75f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x + elem->getDimension().x, -elem->getPosition().y, UI_Z, 1.0f );
    v->color = mBGColor;
    v->tex = XMFLOAT2( 1.0f, 0.75f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x + elem->getDimension().x, -elem->getPosition().y - elem->getDimension().y, UI_Z, 1.0f );
    v->color = mBGColor;
    v->tex = XMFLOAT2( 1.0f, 0.765625f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x, -elem->getPosition().y - elem->getDimension().y, UI_Z, 1.0f );
    v->color = mBGColor;
    v->tex = XMFLOAT2( 0.0f, 0.765625f );
    v++;

    mVertsGenerated += 4;
}