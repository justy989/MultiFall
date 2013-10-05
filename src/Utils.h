#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>
#include <xnamath.h>

//Defines
#define LIMIT_60_FPS 1.0f / 60.0f

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
#define SafeDelete(x) { delete x; x = 0; }

#define CLAMP(x, min, max) if( x < min ){x = min;} if (x > max){x = max;}

#define EPSILON 0.00001f

#define BYTE_MAX 255

#define MAX_EMITTERS 50
#define MAX_PARTICLES 100

#define PI_OVER_2 ( 3.14159f / 2.0f )

//Typedefs
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;

struct PID
{
	XMFLOAT3 Pos;
	float Rot;
	XMFLOAT4 isAlive;
};

//Structs
struct ParticleInstanceData
{
	PID data[MAX_PARTICLES];
};

struct DungeonVertex{
    XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 tex;

    static void createSurfaceNormals( DungeonVertex& a, DungeonVertex& b, DungeonVertex& c )
    {
	    XMFLOAT3 normal;

        //Load the vectors
	    XMVECTOR v1 = XMLoadFloat3(&XMFLOAT3(a.position.x, a.position.y, a.position.z));
	    XMVECTOR v2 = XMLoadFloat3(&XMFLOAT3(b.position.x, b.position.y, b.position.z));
	    XMVECTOR v3 = XMLoadFloat3(&XMFLOAT3(c.position.x, c.position.y, c.position.z));

        //Find the normal
	    XMVECTOR n  = XMVector3Cross(XMVectorSubtract(v2 ,v1), XMVectorSubtract(v3 ,v1));

        //Normalize it and store it
	    n = XMVector3Normalize(n);
	    XMStoreFloat3(&normal, n);

        //Side effect, a, b, and c have their normals set
	    a.normal = normal;
	    b.normal = normal;
	    c.normal = normal;
    }
};

#endif