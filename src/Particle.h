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
	void setSize(float size);

	void setMaxTimeAlive(float maxTime);
	void setAlive();

	void Update(float dt);

	inline XMFLOAT3 getGravity();
	inline XMFLOAT3 getVelocity();
	inline XMFLOAT3 getPosition();
	inline float getAngle();
	inline float getAngularVelocity();
	inline float getSize();
	inline bool isAlive();

protected:
	XMFLOAT3 mGravity, mVelocity, mPosition;
	float mAngle, mAngularVelocity, mSize;
	
	bool mIsAlive;

	float mTimeAlive;
	float mMaxTimeAlive;
};

inline XMFLOAT3 Particle::getGravity(){return mGravity;}
inline XMFLOAT3 Particle::getVelocity(){return mVelocity;}
inline XMFLOAT3 Particle::getPosition(){return mPosition;}
inline float Particle::getAngle(){return mAngle;}
inline float Particle::getAngularVelocity(){return mAngularVelocity;}
inline float Particle::getSize(){return mSize;}
inline bool Particle::isAlive(){return mIsAlive;}