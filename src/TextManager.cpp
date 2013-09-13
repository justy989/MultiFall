#include "TextManager.h"
#include "Log.h"
#include "Utils.h"

TextManager::TextManager() :
	mFontRV(NULL),

	mVertexBuffer(NULL),
	mIndexBuffer(NULL),

	mWorldCB(NULL)
{
	
}

bool TextManager::init(ID3D11Device* device)
{
	if( FAILED( D3DX11CreateShaderResourceViewFromFile( device,
        L"content/fonts/gamefont.png", 0, 0, &mFontRV, 0 ) ) ){
        LOG_ERRO << "Failed to create Shader resource out of: " << LOG_WC_TO_C(L"content/fonts/gamefont.png") << LOG_ENDL;
        return false;
    }

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory( &resourceData, sizeof( resourceData ) );

	//clockwise
	WORD indices[] =
	{
		0,  1,  3,  
		1,  2,  3
	};

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory( &indexDesc, sizeof( indexDesc ) );
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof( WORD ) * 6;
	indexDesc.CPUAccessFlags = 0;
	resourceData.pSysMem = indices;
    
	device->CreateBuffer( &indexDesc, &resourceData, &mIndexBuffer );

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	unsigned int totalLayoutElements = ARRAYSIZE( vertexDesc );

	//shader variables to be updated
	D3D11_BUFFER_DESC constDesc;
    ZeroMemory( &constDesc, sizeof( constDesc ) );
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.ByteWidth = sizeof( XMMATRIX );
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    
	if( FAILED( device->CreateBuffer( &constDesc, 0, &mWorldCB ) ) ){
        LOG_ERRO << "Failed to create constant buffer for Text" << LOG_ENDL;
        return false;
    }

    LOG_INFO << "Loaded Text Texture and Shader Successfully" << LOG_ENDL;
    return true;
}

void TextManager::clear()
{
	ReleaseCOM(mFontRV);

	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);

	ReleaseCOM(mWorldCB);

    LOG_INFO << "Released Text Assets and Buffers" << LOG_ENDL;
}

void TextManager::setupBuffers(ID3D11Device* device, std::string text)
{
	//each char in the string needs it's own quad
	int length = text.length()*4;

    //If an empty string is given, we don't need to create buffers
    if( !length ){
        return;
    }

	FontVertex* fontQuads = new FontVertex[length];

	for(int i = 0; i < text.length(); i++)
	{
		//set default font color
		fontQuads[i * 4].color		= XMFLOAT4(1,1,1,1);
		fontQuads[i * 4 + 1].color	= XMFLOAT4(1,1,1,1);
		fontQuads[i * 4 + 2].color	= XMFLOAT4(1,1,1,1);
		fontQuads[i * 4 + 3].color	= XMFLOAT4(1,1,1,1);

		//set font location starting at -1, 1 (top left)
		//and move right (no wrapping)
        //Set depth to be 0.01 to be in front of UI
		fontQuads[i * 4].position		= XMFLOAT4((i*FONTWIDTH),				0.0f,			0.0f, 1);
		fontQuads[i * 4 + 1].position	= XMFLOAT4((i*FONTWIDTH) + FONTWIDTH,	0.0f,			0.0f, 1);
		fontQuads[i * 4 + 2].position	= XMFLOAT4((i*FONTWIDTH),				-FONTHEIGHT,	0.0f, 1);
		fontQuads[i * 4 + 3].position	= XMFLOAT4((i*FONTWIDTH) + FONTWIDTH,	-FONTHEIGHT,	0.0f, 1);

		setFontQuad(text.at(i), &fontQuads[i*4]);
	}

	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(FontVertex) * (length);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = fontQuads;

    device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);

	int v = 0;
    int index = 0;

	ushort* inds = new ushort[ 6 * text.length() ];

	//Generate indices corresponding to generated verts
	for(int i = 0; i < text.length(); i++){       
            inds[v] = index;
            inds[v+1] = index + 1;
            inds[v+2] = index + 2;

            inds[v+3] = index + 1;
            inds[v+4] = index + 3;
            inds[v+5] = index + 2;

            v += 6;
            index += 4;
    }

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(ushort) * text.length() * 6;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = inds;

    //Create VB
    device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);

    delete[] fontQuads;
    delete[] inds;
}

void TextManager::drawString(ID3D11DeviceContext* device, std::string text, float x, float y)
{
	ID3D11Device* d;
	device->GetDevice(&d);
	setupBuffers(d, text);	

	UINT stride = sizeof(FontVertex);
    UINT offset = 0;

	device->IASetIndexBuffer( mIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
	device->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	XMMATRIX world = XMMatrixTranslation(x, -y, 0.0501f);
    XMMATRIX worldMat = world;
    worldMat = XMMatrixTranspose( worldMat );

	device->PSSetShaderResources( 0, 1, &mFontRV );

	device->UpdateSubresource( mWorldCB, 0, 0, &worldMat, 0, 0 );
	device->VSSetConstantBuffers( 0, 1, &mWorldCB );

	device->DrawIndexed(6 * text.length(), 0, 0);
}

bool TextManager::setFontQuad(char c, FontVertex* fontQuad)
{
	//our font starts with ascii code (in dec) 33
	//and ends at 126
	int ascii = (int)c;
	if(ascii < 33 || ascii > 126)
	{
		return false;
	}

	//in pixels
	float imgWidth	= 512;
	float imgHeight = 256;

	int charWidth	= 30;
	int charHeight	= 34;

	//in chars
	int gridWidth	= 17;

	int row		= (ascii - 33) / gridWidth;
	int column	= (ascii - 33) % gridWidth;

	float dx = ((float)charWidth) / imgWidth;
	float dy = ((float)charHeight) / imgHeight;

	//get texcoords
	fontQuad[0].tex.x = (float)(column * dx);
	fontQuad[0].tex.y = (float)(row * dy);

	fontQuad[1].tex.x = (float)(column * dx) + dx;
	fontQuad[1].tex.y = (float)(row * dy);

	fontQuad[2].tex.x = (float)(column * dx);
	fontQuad[2].tex.y = (float)(row * dy)    + dy;

	fontQuad[3].tex.x = (float)(column * dx) + dx;
	fontQuad[3].tex.y = (float)(row * dy)	 + dy;

	return true;
}