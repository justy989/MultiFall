#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>
#include <xnamath.h>

#include "Utils.h"

#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>
#include <xnamath.h>

class ParticleDisplay
{

public:
	ParticleDisplay();

	bool init(ID3D11Device* device);

	void Draw(ID3D11DeviceContext* device);

	struct ParticleVertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
protected:
	int mVertexCount, mIndexCount;
	ParticleVertex* mVertices;
	ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
};