#include "Particle.h"

Particle::Particle() :
	mGravity(0,0,0),
	mVelocity(0,0,0),
	mPosition(0,0,0),
	mAngle(0),
	mAngularVelocity(0)
{

}

void Particle::Update(float dt)
{
	mVelocity.x -= mGravity.x * dt;
	mVelocity.y -= mGravity.y * dt;
	mVelocity.z -= mGravity.z * dt;

	mPosition.x += mVelocity.x * dt;
	mPosition.y += mVelocity.y * dt;
	mPosition.z += mVelocity.z * dt;

	mAngle += mAngularVelocity * dt;	
}

void Particle::setGravity(XMFLOAT3 grav)
{
	mGravity = grav;
}
void Particle::setVelocity(XMFLOAT3 vel)
{
	mVelocity = vel;
}
void Particle::setPosition(XMFLOAT3 pos)
{
	mPosition = pos;
}
void Particle::setRotation(float angle)
{
	mAngle = angle;
}
void Particle::setAngularVelocity(float vel)
{
	mAngularVelocity = vel;
}