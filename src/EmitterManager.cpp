#include "EmitterManager.h"

EmitterManager::EmitterManager() :
	mNumEmitters(0)
{

}

void EmitterManager::clear()
{
	for(int i = 0; i < MAX_EMITTERS; i++)
	{
        mEmitters[i].clear();
    }
}

void EmitterManager::Update(float dt)
{
	for(int i = 0; i < MAX_EMITTERS; i++)
	{
		if(mEmitters[i].isAlive())
		{
			mEmitters[i].Update(dt);
		}
	}
}

void EmitterManager::Draw(ID3D11DeviceContext* device)
{
	for(int i = 0; i < MAX_EMITTERS; i++)
	{
		if(mEmitters[i].isAlive())
		{
			mEmitters[i].Draw(device);
		}
	}
}

bool EmitterManager::init(ID3D11Device* device, ID3DX11EffectTechnique* tech)
{
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile( device,
		L"content/textures/particle.png", 0, 0, &mParticle1, 0 );

	hr = D3DX11CreateShaderResourceViewFromFile( device,
		L"content/textures/particle2.png", 0, 0, &mParticle2, 0 );

	mTech = tech;

	mEmitters = new Emitter[MAX_EMITTERS];
	for(int i = 0; i < MAX_EMITTERS; i++)
	{
		mEmitters[i].init(device, tech);
		mEmitters[i].setTexture(mParticle1);
	}

	return true;
}

void EmitterManager::spawnEmitter(XMFLOAT3 gravity, XMFLOAT3 position, XMFLOAT3 velocity, float maxTime)
{
	for(int i = 0; i < MAX_EMITTERS; i++)
	{
		if(!mEmitters[i].isAlive())
		{
			mEmitters[i].setGravity(gravity);
			mEmitters[i].setPosition(position);
			mEmitters[i].setVelocity(velocity);
			mEmitters[i].setMaxTimeAlive(maxTime);
			mEmitters[i].setParticleChancePerFrame(50);
			mEmitters[i].setAlive();
			break;
		}
	}
}