#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>

#include "Utils.h"
#include "Emitter.h"

class EmitterManager
{
public:
	EmitterManager();

	bool init();

	void Update(float dt);

	void Draw(ID3D11DeviceContext* device);
protected:
	ushort mNumEmitters;
	Emitter* mEmitters;
};