#include <DXGI.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>
#include <xnamath.h>

#include "Utils.h"

class ParticleDisplay
{

public:
	ParticleDisplay();

	bool init(ID3D11Device* device, ID3DX11EffectTechnique* technique);

	void Draw(ParticleInstanceData* stuff, ID3D11DeviceContext* device, ID3D11ShaderResourceView* texture);

	struct ParticleVertex
	{
		XMFLOAT3 position;
	};
protected:
	void UpdateBuffers(ParticleInstanceData* pos, ID3D11DeviceContext* device);

	ID3D11InputLayout* mInputLayout;

	int mVertexCount, mIndexCount;
	ParticleVertex* mVertices;
	ID3D11Buffer *mVertexBuffer; //because it's a point list, no index buffer
	ID3D11Buffer* mParticleInstanceDataCB;
};