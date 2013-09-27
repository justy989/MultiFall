#include "Emitter.h"

Emitter::Emitter() :
	mGravity(0,0,0),
	mVelocity(0,0,0),
	mPosition(0,0,0),
	mTimeAlive(0),
	mIsAlive(false)
{

}

bool Emitter::init(ID3D11Device* device, ID3DX11EffectTechnique* technique)
{
	mParticleDisplay.init(device, technique);

	return true;
}

void Emitter::setTexture(ID3D11ShaderResourceView* srv)
{
	mTexture = srv;
}

void Emitter::setMaxTimeAlive(float maxTime)
{
	mMaxTimeAlive = maxTime;
}

void Emitter::setAlive()
{
	mTimeAlive = 0;
	mIsAlive = true;
}

void Emitter::Draw(ID3D11DeviceContext* device)
{
	mParticleDisplay.Draw(getLightParticleInstanceData(), device);
}

void Emitter::Update(float dt)
{
	if(mTimeAlive > mMaxTimeAlive)
	{
		mIsAlive = false;
		return;
	}
	mVelocity.x -= mGravity.x * dt;
	mVelocity.y -= mGravity.y * dt;
	mVelocity.z -= mGravity.z * dt;

	mPosition.x += mVelocity.x * dt;
	mPosition.y += mVelocity.y * dt;
	mPosition.z += mVelocity.z * dt;

	for(int i = 0; i < mNumDarkParticles; i++)
	{
		mDarkParticles[i].Update(dt);
	}

	for(int i = 0; i < mNumLightParticles; i++)
	{
		mLightParticles[i].Update(dt);
	}
}

void Emitter::setGravity(XMFLOAT3 grav)
{
	mGravity = grav;
}
void Emitter::setVelocity(XMFLOAT3 vel)
{
	mVelocity = vel;
}
void Emitter::setPosition(XMFLOAT3 pos)
{
	mPosition = pos;
}