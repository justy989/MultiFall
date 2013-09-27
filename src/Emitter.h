#include "Utils.h"
#include "Particle.h"
#include "ParticleDisplay.h"

#include <D3DX11.h>
#include <D3D11.h>

class Emitter
{
public:
	Emitter();

	void setGravity(XMFLOAT3 grav);
	void setVelocity(XMFLOAT3 vel);
	void setPosition(XMFLOAT3 pos);
	void setMaxTimeAlive(float maxTime);
	void setTexture(ID3D11ShaderResourceView* srv);
	void setAlive();

	bool init(ID3D11Device* device, ID3DX11EffectTechnique* technique);

	void Draw(ID3D11DeviceContext* device);

	void Update(float dt);

	inline ushort getNumLightParticles();
	inline ushort getNumDarkParticles();

	inline Particle* getLightParticles();
	inline Particle* getDarkParticles();

	inline ParticleInstanceData* getLightParticleInstanceData();
	inline ParticleInstanceData* getDarkParticlePositions();

	inline bool isAlive();

protected:
	XMFLOAT3 mGravity, mVelocity, mPosition;
	ParticleInstanceData stuff[MAX_PARTICLES];

	ParticleDisplay mParticleDisplay;
	ID3D11ShaderResourceView* mTexture;

	bool mIsAlive;

	float mTimeAlive;
	float mMaxTimeAlive;


	ushort mNumLightParticles;
	Particle* mLightParticles;

	ushort mNumDarkParticles;
	Particle* mDarkParticles;
};

inline ushort Emitter::getNumLightParticles(){return mNumLightParticles;}
inline ushort Emitter::getNumDarkParticles(){return mNumDarkParticles;}

inline Particle* Emitter::getLightParticles(){return mLightParticles;}
inline Particle* Emitter::getDarkParticles(){return mDarkParticles;}

inline bool Emitter::isAlive(){return mIsAlive;}

inline ParticleInstanceData* Emitter::getLightParticleInstanceData()
{
	for(int i = 0; i<mNumLightParticles; i++)
	{
		stuff[i].Pos = mLightParticles[i].getPosition();
		stuff[i].Rot = mLightParticles[i].getAngle();
		stuff[i].isAlive = XMFLOAT4(mLightParticles[i].isAlive(),0,0,0);
	}

	return stuff;
}
inline ParticleInstanceData* Emitter::getDarkParticlePositions()
{
	for(int i = 0; i<mNumDarkParticles; i++)
	{
		stuff[i].Pos = mDarkParticles[i].getPosition();
		stuff[i].Rot = mDarkParticles[i].getAngle();
		stuff[i].isAlive = XMFLOAT4(mDarkParticles[i].isAlive(),0,0,0);
	}

	return stuff;
}