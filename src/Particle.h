#include "Utils.h"

class Particle
{
public:
	Particle();

	void setGravity(XMFLOAT3 grav);
	void setVelocity(XMFLOAT3 vel);
	void setPosition(XMFLOAT3 pos);
	void setRotation(float angle);
	void setAngularVelocity(float vel);

	void Update(float dt);
protected:
	XMFLOAT3 mGravity, mVelocity, mPosition;
	float mAngle, mAngularVelocity;
};