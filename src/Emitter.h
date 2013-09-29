#include "Utils.h"
#include "Particle.h"
#include "ParticleDisplay.h"
#include "Random.h"

#include <D3DX11.h>
#include <D3D11.h>

class Emitter
{
public:
	Emitter();

	void setGravity(XMFLOAT3 grav);
	void setVelocity(XMFLOAT3 vel);
	void setPosition(XMFLOAT3 pos);	
	void setParticleChancePerFrame(ushort percentage);
	void setTexture(ID3D11ShaderResourceView* srv);
	void setMaxTimeAlive(float maxTime);
	void setAlive();

	bool init(ID3D11Device* device, ID3DX11EffectTechnique* technique);

	void Draw(ID3D11DeviceContext* device);

	void Update(float dt);

	inline ushort getNumLightParticles();
	inline ushort getNumDarkParticles();

	inline Particle* getLightParticles();
	inline Particle* getDarkParticles();

	inline void setLightParticleInstanceData();
	inline void setDarkParticlePositions();

	inline bool isAlive();

protected:
	XMFLOAT3 mGravity, mVelocity, mPosition;
	ParticleInstanceData stuff;

	ParticleDisplay mParticleDisplay;
	ID3D11ShaderResourceView* mTexture;

	bool mIsAlive;

	float mTimeAlive;
	float mMaxTimeAlive;

	ushort mPercentage;


	ushort mNumLightParticles;
	Particle mLightParticles[MAX_PARTICLES];

	ushort mNumDarkParticles;
	Particle mDarkParticles[MAX_PARTICLES];

	Random mRand;
};

inline ushort Emitter::getNumLightParticles(){return mNumLightParticles;}
inline ushort Emitter::getNumDarkParticles(){return mNumDarkParticles;}

inline Particle* Emitter::getLightParticles(){return mLightParticles;}
inline Particle* Emitter::getDarkParticles(){return mDarkParticles;}

inline bool Emitter::isAlive(){return mIsAlive;}

inline void Emitter::setLightParticleInstanceData()
{
	for(int i = 0; i<MAX_PARTICLES; i++)
	{
		stuff.data[i].Pos = mLightParticles[i].getPosition();
		stuff.data[i].Rot = mLightParticles[i].getAngle();
		stuff.data[i].isAlive = XMFLOAT4(mLightParticles[i].isAlive(),0,0,0);
	}
}
inline void Emitter::setDarkParticlePositions()
{
	for(int i = 0; i<MAX_PARTICLES; i++)
	{
		stuff.data[i].Pos = mDarkParticles[i].getPosition();
		stuff.data[i].Rot = mDarkParticles[i].getAngle();
		stuff.data[i].isAlive = XMFLOAT4(mDarkParticles[i].isAlive(),0,0,0);
	}
}