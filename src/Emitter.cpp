#include "Emitter.h"

#include <time.h>

Emitter::Emitter() :
	mGravity(0,0,0),
	mVelocity(0,0,0),
	mPosition(0,0,0),
	mTimeAlive(0),
	mPercentage(50),
	mIsAlive(false)
{

}

bool Emitter::init(ID3D11Device* device, ID3DX11EffectTechnique* technique)
{
	mParticleDisplay.init(device, technique);

	mRand.seed(time(0));

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
	setLightParticleInstanceData();
	mParticleDisplay.Draw(&stuff, device, mTexture);
}

void Emitter::setParticleChancePerFrame(ushort percentage)
{
	mPercentage = percentage;
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

	//for(int i = 0; i < mNumDarkParticles; i++)
	//{
	//	if(mDarkParticles[i].isAlive())
	//	{
	//		mDarkParticles[i].Update(dt);
	//	}		
	//}

	if(mRand.gen(0, 100) <= mPercentage)
	{
		for(int i = 0; i < MAX_PARTICLES; i++)
		{
			if(!mLightParticles[i].isAlive())
			{
				float scale = 0.3f;
				mLightParticles[i].setGravity(XMFLOAT3(0,0.5f,0));
				mLightParticles[i].setPosition(mPosition);
				mLightParticles[i].setVelocity(XMFLOAT3(scale*(mRand.genNorm()-0.5f),scale*(mRand.genNorm()-0.5f),scale*(mRand.genNorm()-0.5f)));
				mLightParticles[i].setMaxTimeAlive(3.0f); //temp value for now
				mLightParticles[i].setAlive();
				break;
			}
			
		}
	}

	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		if(mLightParticles[i].isAlive())
		{
			mLightParticles[i].Update(dt);
		}
	}	

	mTimeAlive+=dt;
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