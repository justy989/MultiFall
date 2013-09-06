#ifndef WORLD_H
#define WORLD_H

#include "Environment.h"
#include "WorldEntity.h"

class World{
public:

    World();

    void moveEntity( WorldEntity* entity, XMVECTOR moveVec, float dist );

    void update( float dt );

    inline Environment& getEnv();

protected:

    bool checkEntityCollision( WorldEntity* entity, XMVECTOR desiredPosition, XMVECTOR* outWallVector);

    Environment mEnv;
    //Population mPop;
};

Environment& World::getEnv(){return mEnv;}

#endif