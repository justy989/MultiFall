#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <dxgi.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <string>

#define FONTWIDTH	0.04
#define FONTHEIGHT  0.046

struct FontVertex
{
	XMFLOAT4 position;
	XMFLOAT4 color;
	XMFLOAT2 tex;
};

class TextManager
{
public:
	TextManager();

	bool init(ID3D11Device* device);

    void clear();

	//Draw string directly to the screen
	void drawString(ID3D11DeviceContext* device, std::string text, float x, float y, XMFLOAT4 color = XMFLOAT4(1,1,1,1) );

protected:

	void setupBuffers(ID3D11Device* device, std::string text, XMFLOAT4 color);

	bool setFontQuad(char c, FontVertex* fontQuad);

protected:

	ID3D11ShaderResourceView* mFontRV;

	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	ID3D11Buffer* mWorldCB;
};

#endif