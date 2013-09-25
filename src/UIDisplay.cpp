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
    mBorderDimension(0.01f),
    mBuildingElements(false)
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
    ReleaseCOM( mInputLayout );

    LOG_INFO << "Released UI Buffers, textures, shaders, sampler, and input layout" << LOG_ENDL;
}

void UIDisplay::buildWindowVB( UIWindow& window, float aspectRatio )
{
    //Build window border
    buildBorderVB( &window, aspectRatio );

    if( window.getTabCount() > 1 ){
        //Buil Tabs after subtracting the title bar and border
        buildTabVB( window, aspectRatio );
    }else{
        //Calculate the inside of a window
        UIWindow w;
        float borderWidth = mBorderDimension;
        float borderHeight = borderWidth * aspectRatio;

        w.setPosition( XMFLOAT2(window.getPosition().x + borderWidth, (window.getPosition().y + UIWINDOW_TITLE_BAR_HEIGHT) ) );
        w.setDimension( XMFLOAT2(window.getDimension().x - ( borderWidth * 2.0f ), (window.getDimension().y - UIWINDOW_TITLE_BAR_HEIGHT ) - borderHeight ) );
        buildBorderVB( &w, aspectRatio );
    }

    //Build BG
    buildBGVB( &window, mBGColor );

    UIWindow::Tab& t = window.getTab( window.getCurrentTab() );

    mBuildingElements = true;

    //Loop through and build elements in the current tab
    for(int i = 0; i < t.elementCount; i++){
        buildBorderVB( t.elements[i], aspectRatio, window.getPosition() );

        if( t.elements[i]->getElemType() == UIElement::ElemType::Slider ){
            UISlider* slider = *(UISlider**)(t.elements + i);

            UIWindow w;
            w.setPosition( XMFLOAT2( slider->getPosition().x + window.getPosition().x, slider->getPosition().y + window.getPosition().y ) );
            w.setDimension( XMFLOAT2( slider->getDimension().x * slider->getPercent(), slider->getDimension().y ) );

            XMFLOAT4 pctColor = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.3f );

            buildBGVB( &w, pctColor );
        }else if( t.elements[i]->getElemType() == UIElement::ElemType::CheckBox ){
            UICheckbox* cb = *(UICheckbox**)(t.elements + i);

            if( cb->isChecked() ){
                UICheckbox b;

                b.setPosition( XMFLOAT2( cb->getPosition().x + window.getPosition().x, cb->getPosition().y + window.getPosition().y ) );
                b.setDimension( XMFLOAT2( cb->getDimension().x, cb->getDimension().y ) );
                buildCheckboxVB( &b );
            }
        }
    }

    mBuildingElements = false;
}

void UIDisplay::drawWindowText( ID3D11DeviceContext* device, UIWindow& window, TextManager& tm )
{
    UIWindow::Text* text;
    uint textCount;
    window.getText( &text, &textCount );

    float x = 0.0f;
    float y = 0.0f;
    float len = static_cast<float>( strlen( text->message ) ) * FONTWIDTH;

    y = window.getPosition().y + (UIWINDOW_TITLE_BAR_HEIGHT / 2.0f) - ( FONTHEIGHT / 2.0f );
    x = window.getPosition().x + ( window.getDimension().x / 2.0f ) - ( len / 2.0f );

    XMFLOAT4 colors[3] = 
    {
        XMFLOAT4(1,1,1,1), //Not selected: White
        XMFLOAT4(1,1,0,1), //Highlighted: Yellow
        XMFLOAT4(0,0,1,1)  //Selected: Blue
    };

    D3DX11_TECHNIQUE_DESC techDesc;
	mTechnique->GetDesc( &techDesc );

	for(ushort p = 0; p < techDesc.Passes; ++p)
	{
        //Draw title
        mTechnique->GetPassByIndex(p)->Apply(0, device);
        tm.drawString( device, text->message, x, y );

        float len = 0.0f;

        //Draw tab names
        for(uint i = 0; i < window.getTabCount(); i++){
            tm.drawString( device, window.getTab(i).name, 
                window.getPosition().x +(mBorderDimension * 2.0f) + len, 
                window.getPosition().y + (UIWINDOW_TITLE_BAR_HEIGHT * 2.0f) - ( FONTHEIGHT * 1.5f ),
                i == window.getCurrentTab() ? colors[2] : window.getTab(i).highlighted ? colors[1] : colors[0]);
            len += static_cast<float>(strlen(window.getTab(i).name)) * FONTWIDTH + ( FONTWIDTH / 2.0f );
        }

        //Draw UI Text
        UIWindow::Tab& t = window.getTab( window.getCurrentTab() );

        //Loop through and build elements in the current tab
        for(int i = 0; i < t.elementCount; i++){

            if( t.elements[i]->getElemType() == UIElement::ElemType::TextBox ){
                UIElement::Text* text;
                uint textCount;

                t.elements[i]->getText( &text, &textCount );

                for(uint l = 0; l < textCount; l++){
                    tm.drawString( device, text[l].message,
                        t.elements[i]->getPosition().x + text[l].offset.x + window.getPosition().x + FONTWIDTH / 2.0f, 
                        t.elements[i]->getPosition().y + text[l].offset.y + window.getPosition().y + (FONTHEIGHT / 2.0f),
                        colors[0] );
                }
            }else{
                UIElement::Text* text;
                uint tCount;
                t.elements[i]->getText(&text, &tCount );

                //Center the text
                float xOffset = (t.elements[i]->getDimension().x / 2.0f) - (static_cast<float>(strlen(text->message)) * FONTWIDTH / 2.0f);
                float yOffset = (t.elements[i]->getDimension().y / 2.0f) -  FONTHEIGHT / 2.0f;

                //Draw the text bro
                tm.drawString( device, text->message,
                    t.elements[i]->getPosition().x + window.getPosition().x + xOffset + text->offset.x, 
                    t.elements[i]->getPosition().y + window.getPosition().y + yOffset + text->offset.y,
                    colors[ t.elements[i]->getSelectedState() ]);
            }

            //If it is a drop menu, draw the options
            if( t.elements[i]->getElemType() == UIElement::ElemType::DropMenu ){
                UIDropMenu* d = *(UIDropMenu**)(t.elements + i);

                float tx = t.elements[i]->getPosition().x + window.getPosition().x + (FONTWIDTH / 2.0f);
                float ty = t.elements[i]->getPosition().y + window.getPosition().y + (FONTHEIGHT / 2.0f);

                if( d->isDropped() ){
                    for(uint o = 0; o < d->getOptionCount(); o++){
                        tm.drawString( device, d->getOption(o),
                            tx, ty, 
                            d->getSelectedOption() == o ? colors[2] : d->getHightlightedOption() == o ? colors[1] : colors[0] );
                        ty += 0.1f;
                    }
                }else{
                    tm.drawString( device, d->getOption(d->getSelectedOption()),
                    tx, ty, 
                    colors[2]);
                }
            }else if( t.elements[i]->getElemType() == UIElement::ElemType::InputBox ){
                UIInputBox* d = *(UIInputBox**)(t.elements + i);

                float tx = t.elements[i]->getPosition().x + window.getPosition().x + (FONTWIDTH / 2.0f);
                float ty = t.elements[i]->getPosition().y + window.getPosition().y + (FONTHEIGHT / 2.0f);

                tm.drawString( device, d->getInput(),
                    tx, ty, 
                    colors[0]);
            }
        }
    }
}


void UIDisplay::drawUI( ID3D11DeviceContext* device )
{
    if( mVertsGenerated > 0 ){
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
        
        if( mVertsGenerated > 0 ){
            //Draw the verts we generated wooo
            device->DrawIndexed( indices, 0, 0 );
        }
    }

    mVertsGenerated = 0;

    ReleaseCOM(mVB);
}

void UIDisplay::buildBorderVB( UIElement* elem, float aspectRatio, XMFLOAT2 offset )
{
    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    XMFLOAT2 offSetPos;
    XMFLOAT2 savePos;

    offSetPos.x = elem->getPosition().x + offset.x;
    offSetPos.y = elem->getPosition().y + offset.y;

    savePos = elem->getPosition();

    elem->setPosition( offSetPos );

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

    elem->setPosition( savePos );
}


void UIDisplay::buildTabVB( UIWindow& window, float aspectRatio )
{
    float borderWidth = mBorderDimension;
    float borderHeight = borderWidth * aspectRatio;

    float l = window.getPosition().x + borderWidth;
    float t = -window.getPosition().y - UIWINDOW_TITLE_BAR_HEIGHT;
    float r = l + borderWidth;
    float b = t - borderHeight;

    //Top Left corner
    buildCornerVB( l, t, aspectRatio );

    //Build Top Left Bar
    buildLeftBarVB( b, b - UIWINDOW_TITLE_BAR_HEIGHT + borderHeight, l, aspectRatio );

    //Tab Borders
    for(uint i = 0; i < window.getTabCount(); i++){
        float len = static_cast<float>(strlen(window.getTab(i).name)) * FONTWIDTH;
        float saveCorner = l;

        //Move over our box
        l = r;
        r = r + len;

        //Top Bar for tab
        buildTopBarVB( l, r, t, aspectRatio );

        l = r;
        r += borderWidth;

        //Top Right Corner for tab
        buildCornerVB( l, t, aspectRatio );

        b = t - UIWINDOW_TITLE_BAR_HEIGHT;

        //Right bar for tab
        buildRightBarVB( t, b, l, aspectRatio );

        //Draw the top bars on either side of the current tab
        if( i == window.getCurrentTab() ){
            //Left Top Bar
            buildTopBarVB( window.getPosition().x + ( borderWidth * 2.0f ), saveCorner, b, aspectRatio );
            buildCornerVB( saveCorner, b, aspectRatio );

            //Right top bar
            buildTopBarVB( r, window.getPosition().x + window.getDimension().x - (borderWidth * 2.0f), b, aspectRatio );
            buildCornerVB( l, b, aspectRatio );
        }
    }

    l = window.getPosition().x + window.getDimension().x - ( borderWidth * 2.0f );
    r = l + borderWidth;

    //buildTopBarVB( l, r, b, aspectRatio );

    //Top Right Corner
    //l = r;
    //r += borderWidth;

    buildCornerVB( l, b, aspectRatio );

    //RB
    t = b;
    b = (-window.getPosition().y - window.getDimension().y) + (borderHeight * 2.0f);

    buildRightBarVB( t, b, l, aspectRatio );

    //BRC
    t = b;
    b -= borderHeight;

    buildCornerVB( l, t, aspectRatio );

    //BB
    l = window.getPosition().x + ( borderWidth * 2.0f ); 
    r = (window.getPosition().x + window.getDimension().x) - (borderWidth * 2.0f);

    buildBottomBarVB( l, r, t, aspectRatio );

    //BLC
    l = window.getPosition().x + borderWidth;
    r = l + borderWidth;

    buildCornerVB( l, t, aspectRatio );

    //LB
    t = -window.getPosition().y - ( UIWINDOW_TITLE_BAR_HEIGHT * 2.0f) - borderHeight;
    b = (-window.getPosition().y - window.getDimension().y) + borderHeight;

    buildLeftBarVB( t, b, l, aspectRatio );

    t += borderHeight;

    //Top Left Corner
    buildCornerVB( l, t, aspectRatio );

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

    v->position = XMFLOAT4( l, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( r, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( r, b, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( l, b, UI_DETERMINE_Z, 1.0f );
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

    v->position = XMFLOAT4( l, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( r, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( r, b, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( l, b, UI_DETERMINE_Z, 1.0f );
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

    v->position = XMFLOAT4( l, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( r, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( r, b, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( l, b, UI_DETERMINE_Z, 1.0f );
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

    v->position = XMFLOAT4( l, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_BOTTOM );
    v++;

    v->position = XMFLOAT4( r, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( r, b, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( wrap, UI_BAR_TOP );
    v++;

    v->position = XMFLOAT4( l, b, UI_DETERMINE_Z, 1.0f );
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

    v->position = XMFLOAT4( l, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_CORNER_TOP );
    v++;

    v->position = XMFLOAT4( r, t, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, UI_CORNER_TOP );
    v++;

    v->position = XMFLOAT4( r, b , UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.25f, UI_CORNER_BOTTOM );
    v++;

    v->position = XMFLOAT4( l, b, UI_DETERMINE_Z, 1.0f );
    v->color = c;
    v->tex = XMFLOAT2( 0.0f, UI_CORNER_BOTTOM );
    v++;

    mVertsGenerated += 4;
}

void UIDisplay::buildBGVB( UIElement* elem, XMFLOAT4& color )
{
    FontVertex* v = mVerts + mVertsGenerated;

    v->position = XMFLOAT4( elem->getPosition().x, -elem->getPosition().y, UI_DETERMINE_Z, 1.0f );
    v->color = color;
    v->tex = XMFLOAT2( 0.0f, 0.75f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x + elem->getDimension().x, -elem->getPosition().y, UI_DETERMINE_Z, 1.0f );
    v->color = color;
    v->tex = XMFLOAT2( 1.0f, 0.75f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x + elem->getDimension().x, -elem->getPosition().y - elem->getDimension().y, UI_DETERMINE_Z, 1.0f );
    v->color = color;
    v->tex = XMFLOAT2( 1.0f, 0.765625f );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x, -elem->getPosition().y - elem->getDimension().y, UI_DETERMINE_Z, 1.0f );
    v->color = color;
    v->tex = XMFLOAT2( 0.0f, 0.765625f );
    v++;

    mVertsGenerated += 4;
}

void UIDisplay::buildCheckboxVB( UICheckbox* elem )
{
    FontVertex* v = mVerts + mVertsGenerated;
    XMFLOAT4 color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );

    v->position = XMFLOAT4( elem->getPosition().x, -elem->getPosition().y, UI_DETERMINE_Z, 1.0f );
    v->color = color;
    v->tex = XMFLOAT2( UI_CHECKBOX_LEFT, UI_CHECKBOX_TOP );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x + elem->getDimension().x, -elem->getPosition().y, UI_DETERMINE_Z, 1.0f );
    v->color = color;
    v->tex = XMFLOAT2( UI_CHECKBOX_RIGHT, UI_CHECKBOX_TOP );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x + elem->getDimension().x, -elem->getPosition().y - elem->getDimension().y, UI_DETERMINE_Z, 1.0f );
    v->color = color;
    v->tex = XMFLOAT2( UI_CHECKBOX_RIGHT, UI_CHECKBOX_BOTTOM );
    v++;

    v->position = XMFLOAT4( elem->getPosition().x, -elem->getPosition().y - elem->getDimension().y, UI_DETERMINE_Z, 1.0f );
    v->color = color;
    v->tex = XMFLOAT2( UI_CHECKBOX_LEFT, UI_CHECKBOX_BOTTOM );
    v++;

    mVertsGenerated += 4;
}