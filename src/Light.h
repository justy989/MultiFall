#include "Utils.h"

class Light
{
public:
	Light();

	float radius;
	float intensity;
	XMFLOAT3 position;
	XMFLOAT3 direction;	
};