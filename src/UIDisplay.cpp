#include "UIDisplay.h"
#include "Log.h"

#include "D3DUtils.h"

UIDisplay::UIDisplay() :
    mVB(NULL),
    mIB(NULL),
    mTexture(NULL),
    mPixelShader(NULL),
    mVertexShader(NULL),
    mSampler(NULL),
    mInputLayout(NULL),
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

    //shader variables to be updated
	D3D11_BUFFER_DESC constDesc;
    ZeroMemory( &constDesc, sizeof( constDesc ) );
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof( XMMATRIX );
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mWorldCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for UI" << LOG_ENDL;
        return false;
    }

    //Compile the shaders
    ID3DBlob* vsBuffer = 0;

	if( !CompileD3DShader( uiShaderPath, "VS_Main", "vs_5_0", &vsBuffer ) ){
        LOG_ERRO << "Failed to compile: " << LOG_WC_TO_C(uiShaderPath) << " vertex shader" << LOG_ENDL;
        return false;
    }

	device->CreateVertexShader( vsBuffer->GetBufferPointer( ),
	vsBuffer->GetBufferSize( ), 0, &mVertexShader );	 

	ID3DBlob* psBuffer = 0;

	if( !CompileD3DShader( uiShaderPath, "PS_Main", "ps_5_0", &psBuffer ) ){
        LOG_ERRO << "Failed to compile: " << LOG_WC_TO_C(uiShaderPath) << " pixel shader" << LOG_ENDL;
        return false;
    }

	device->CreatePixelShader( psBuffer->GetBufferPointer( ),
	psBuffer->GetBufferSize( ), 0, &mPixelShader );

	psBuffer->Release( );

    //Create the sampler
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; //Point filtering!
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    device->CreateSamplerState( &samplerDesc, &mSampler );

    //Input Description
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	unsigned int totalLayoutElements = ARRAYSIZE( vertexDesc );

	if( FAILED( device->CreateInputLayout(vertexDesc, totalLayoutElements, 
								vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(),
								&mInputLayout) ) ){
        LOG_ERRO << "Failed to create Input Layout for UI" << LOG_ENDL;
        return false;
    }

    vsBuffer->Release();

    D3D11_BLEND_DESC bdesc;
	ZeroMemory(&bdesc, sizeof(D3D11_BLEND_DESC));
	bdesc.RenderTarget[0].BlendEnable = true;
	bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	if( FAILED( device->CreateBlendState(&bdesc, &mBlender) ) ){
        LOG_ERRO << "Failed to create Blend State for UI" << LOG_ENDL;
        return false;
    }

    return true;
}

void UIDisplay::clear()
{
    ReleaseCOM( mVB );
    ReleaseCOM( mIB );
    ReleaseCOM( mTexture );
    ReleaseCOM( mPixelShader );
    ReleaseCOM( mVertexShader );
    ReleaseCOM( mSampler );
    ReleaseCOM( mInputLayout );

    LOG_INFO << "Released UI Buffers, textures, shaders, sampler, and input layout" << LOG_ENDL;
}

void UIDisplay::prepareUIRendering( ID3D11DeviceContext* device )
{
    //Setup input layout and topology
    device->IASetInputLayout( mInputLayout );
    device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    //Setup vertex and pixel shaders
	device->VSSetShader( mVertexShader, 0, 0 );
	device->PSSetShader( mPixelShader, 0, 0 );

    //Setup sampler
	device->PSSetSamplers( 0, 1, &mSampler );

    //Setup blend state
    float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    uint sampleMask = 0xFFFFFFFF;

    device->OMSetBlendState( mBlender, blendFactor, sampleMask );
}

void UIDisplay::buildWindowVB( UIWindow& window, float aspectRatio )
{
    //Build window border
    buildBorderVB( &window, aspectRatio );

    //Build window Body by subtracting the title bar and the border
    UIWindow w;
    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    w.setPosition( XMFLOAT2(window.getPosition().x + borderWidth, (window.getPosition().y + UIWINDOW_TITLE_BAR_HEIGHT) ) );
    w.setDimension( XMFLOAT2(window.getDimension().x - ( borderWidth * 2.0f ), (window.getDimension().y - UIWINDOW_TITLE_BAR_HEIGHT ) - borderHeight ) );

    buildBorderVB( &w, aspectRatio );

    //Build BG
    buildBGVB( &window );

    //Loop through and build elements
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

    tm.DrawString( device, text->message, x, y );
}

void UIDisplay::buildBorderVB( UIElement* elem, float aspectRatio )
{
    //Generate each part of the window
    FontVertex* v = mVerts + mVertsGenerated;
    XMFLOAT4 c = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    float l = elem->getPosition().x;
    float t = -elem->getPosition().y;
    float r = l + borderWidth;
    float b = t - borderHeight;

    //TLC
    v->position = XMFLOAT4( l, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, 0.125f );
    v++;

    v->position = XMFLOAT4( l, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.125f );
    v++;

    mVertsGenerated += 4;

    //TB
    l = r;
    r = ( elem->getPosition().x + elem->getDimension().x ) - borderWidth;

    float wrap = ( r - l ) / ( borderWidth * 4.0f );

    v->position = XMFLOAT4( l, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0f );
    v++;

    v->position = XMFLOAT4( r, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, 0.0f );
    v++;

    v->position = XMFLOAT4( r, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, 0.0625f );
    v++;

    v->position = XMFLOAT4( l, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0625f );
    v++;

    mVertsGenerated += 4;

    //TRC
    l = r;
    r += borderWidth;

    v->position = XMFLOAT4( l, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, 0.125f );
    v++;

    v->position = XMFLOAT4( l, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.125f );
    v++;

    mVertsGenerated += 4;

    //RB
    t = b;
    b = (-elem->getPosition().y - elem->getDimension().y) + borderHeight;

    wrap = ( b - t ) / ( borderHeight * 4.0f );

    v->position = XMFLOAT4( l, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.00f );
    v++;

    v->position = XMFLOAT4( r, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, 0.00f );
    v++;

    v->position = XMFLOAT4( l, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, 0.0625f );
    v++;

    mVertsGenerated += 4;

    //BRC
    t = b;
    b -= borderHeight;

    v->position = XMFLOAT4( l, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, b , 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, 0.125f );
    v++;

    v->position = XMFLOAT4( l, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.125f );
    v++;

    mVertsGenerated += 4;

    //BB
    l = elem->getPosition().x + borderWidth;
    r = (elem->getPosition().x + elem->getDimension().x) - borderWidth;

    wrap = ( r - l ) / ( borderWidth * 4.0f );

    v->position = XMFLOAT4( l, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, 0.0f );
    v++;

    v->position = XMFLOAT4( l, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0f );
    v++;

    mVertsGenerated += 4;

    //BRC
    l = elem->getPosition().x;
    r = l + borderWidth;

    v->position = XMFLOAT4( l, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, b , 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, 0.125f );
    v++;

    v->position = XMFLOAT4( l, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.125f );
    v++;

    mVertsGenerated += 4;

    //LB
    t = -elem->getPosition().y - borderHeight;
    b = (-elem->getPosition().y - elem->getDimension().y) + borderHeight;

    wrap = ( b - t ) / ( borderHeight * 4.0f );

    v->position = XMFLOAT4( l, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, 0.00f );
    v++;

    v->position = XMFLOAT4( r, t, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, 0.0625f );
    v++;

    v->position = XMFLOAT4( r, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.0625f );
    v++;

    v->position = XMFLOAT4( l, b, 0.0f, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, 0.00f );
    v++;

    mVertsGenerated += 4;

    //Draw the title
    /*
    UIWindow::Text* text;
    int tCount;
    elem->getText( &text, &tCount );
    float len = static_cast<float>(strlen( text->message )) * 0.1f;
    tm.DrawString( device, text->message, 
        elem->getPosition().x + ( elem->getDimension().x / 2.0f ) - (len / 2.0f),
        elem->getPosition().y + 0.1f );

    */
}

void UIDisplay::buildBGVB( UIElement* elem )
{
    FontVertex* v = mVerts + mVertsGenerated;

    v->position = XMFLOAT4( elem->getPosition().x, -elem->getPosition().y, 0.0f, 1.0f );
    v->color = mBGColor;
    v->tex = XMFLOAT2( 0.0f, 0.75f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x + elem->getDimension().x, -elem->getPosition().y, 0.0f, 1.0f );
    v->color = mBGColor;
    v->tex = XMFLOAT2( 1.0f, 0.75f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x + elem->getDimension().x, -elem->getPosition().y - elem->getDimension().y, 0.0f, 1.0f );
    v->color = mBGColor;
    v->tex = XMFLOAT2( 1.0f, 0.765625f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x, -elem->getPosition().y - elem->getDimension().y, 0.0f, 1.0f );
    v->color = mBGColor;
    v->tex = XMFLOAT2( 0.0f, 0.765625f );
    v++;

    mVertsGenerated += 4;
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

    //Set vertex and index buffers
    device->IASetVertexBuffers( 0, 1, &mVB, &stride, &offset );
    device->IASetIndexBuffer( mIB, DXGI_FORMAT_R16_UINT, offset );

    //Setup Texture
    device->PSSetShaderResources( 0, 1, &mTexture );

    XMMATRIX world =  XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    XMMATRIX worldTrans = XMMatrixTranspose( world );
    device->UpdateSubresource( mWorldCB, 0, 0, &world, 0, 0 );
	device->VSSetConstantBuffers( 0, 1, &mWorldCB );

    int indices = ( mVertsGenerated / 4 ) * 6;

    //Draw the verts we generated wooo
    device->DrawIndexed( indices, 0, 0 );

    mVertsGenerated = 0;

    ReleaseCOM(mVB);
}