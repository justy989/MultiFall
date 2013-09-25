#include "Emitter.h"

Emitter::Emitter() :
	mGravity(0,0,0),
	mVelocity(0,0,0),
	mPosition(0,0,0),
	mTimeAlive(0)
{

}

void Emitter::Update(float dt)
{
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