#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>

#include "Utils.h"
#include "Emitter.h"

class EmitterManager
{
public:
	EmitterManager();

	bool init(ID3D11Device* device, ID3DX11EffectTechnique* tech);

	void spawnEmitter(XMFLOAT3 gravity, XMFLOAT3 position, XMFLOAT3 velocity, float maxTime);

	void Update(float dt);

	void Draw(ID3D11DeviceContext* device);
protected:
	ushort mNumEmitters;
	Emitter* mEmitters;

	ID3D11ShaderResourceView* mParticle1;
	ID3D11ShaderResourceView* mParticle2;

	ID3DX11EffectTechnique* mTech;
};