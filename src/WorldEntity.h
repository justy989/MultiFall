#ifndef WORLD_ENTITY_H
#define WORLD_ENTITY_H

#include "Utils.h"

class WorldEntity{
public:

    WorldEntity();

    virtual void update( float dt ){}

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
    inline XMFLOAT4& getPosition();
    inline Solidity& getSolidity();

    enum CollisionType{
        None,
        Point,
        Line
    };

    inline void setExistence( Existence existence );

    static bool circleAALineIntersect( XMVECTOR start, XMVECTOR end, XMVECTOR circleCenter, float circleRadius );
    static bool circlePointIntersect( XMVECTOR point, XMVECTOR circleCenter, float circleRadius );

protected:

    //Existence state
    Existence mExistence;
    
    //ID 
    int mID;

    //Solidity for collision
    Solidity mSolidity;

    //Position in the world
    XMFLOAT4 mPosition;
};

inline WorldEntity::Existence WorldEntity::getExistence(){return mExistence;}
inline int WorldEntity::getID(){return mID;}
inline XMFLOAT4& WorldEntity::getPosition(){return mPosition;}
inline WorldEntity::Solidity& WorldEntity::getSolidity(){return mSolidity;}

inline void WorldEntity::setExistence( Existence existence ){mExistence = existence;}

#endif