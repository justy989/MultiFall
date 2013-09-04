#include<d3d11.h>
#include<d3dx11.h>
#include<DxErr.h>
#include<D3Dcompiler.h>
#include <xnamath.h>
#include <string>

class TextManager
{
public:
	TextManager(ID3D11DeviceContext* device);

	//Draw string directly to the screen
	void DrawString(ID3D11DeviceContext* device, std::string text, int x, int y) ;

protected:
	struct FontVertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
		XMFLOAT2 tex;
		XMFLOAT3 padding;
	};

	bool setFontQuad(char c, FontVertex* fontQuad);

	bool CompileD3DShader( LPCWSTR filePath, char* entry, char* shaderModel, ID3DBlob** buffer );

protected:
	ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;

	ID3D11ShaderResourceView* mFontRV;
	ID3D11SamplerState* mSampler;

	ID3D11InputLayout* mInputLayout;

	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
};