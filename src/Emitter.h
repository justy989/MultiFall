#include "Utils.h"
#include "Particle.h"

class Emitter
{
public:
	Emitter();

	void setGravity(XMFLOAT3 grav);
	void setVelocity(XMFLOAT3 vel);
	void setPosition(XMFLOAT3 pos);

	void Update(float dt);
protected:
	XMFLOAT3 mGravity, mVelocity, mPosition;
	float mTimeAlive;

	ushort mNumLightParticles;
	Particle* mLightParticles;

	ushort mNumDarkParticles;
	Particle* mDarkParticles;
};