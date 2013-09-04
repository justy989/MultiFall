#ifndef WORLD_ENTITY_H
#define WORLD_ENTITY_H

#include "Utils.h"

#include <Windows.h>
#include <xnamath.h>

class WorldEntity{
public:

    WorldEntity();

    //virtual void update( float dt ) = 0;

    enum Existence{
        Dead,
        Spawning,
        Alive,
        Dying
    };

    struct Solidity{
        enum BodyType{
            Point, //Use position only
            Sphere, //Use Radius only
            Cylinder, //Use radius and height
            Box //Use width, height and depth
        };

        BodyType type;

        float height;
        float width;

        union{
            float depth;
            float radius;
        };
    };

    inline Existence getExistence();
    inline int getID();
    inline XMFLOAT3& getPosition();

protected:

    //Existence state
    Existence mExistence;
    
    //ID 
    int mID;

    //Solidity for collision
    Solidity mSolidity;

    //Position in the world
    XMFLOAT3 mPosition;
};

inline WorldEntity::Existence WorldEntity::getExistence(){return mExistence;}
inline int WorldEntity::getID(){return mID;}
inline XMFLOAT3& WorldEntity::getPosition(){return mPosition;}

#endif