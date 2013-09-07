#ifndef WORLD_H
#define WORLD_H

#include "Dungeon.h"
#include "WorldEntity.h"

class World{
public:

    World();

    void moveEntity( WorldEntity* entity, XMVECTOR moveVec, float dist );

    void update( float dt );

    //Get the world's dungeon
    inline Dungeon& getDungeon();

    //Get the block dimensions
    inline float getBlockDimension();

    //Set the block dimensions
    inline void setBlockDimension( float d );

protected:

    bool checkEntityCollision( WorldEntity* entity, XMVECTOR desiredPosition, XMVECTOR* outWallVector);

    Dungeon mDungeon;
    //Population mPop;
    //Projectiles mProjs;
    //MagicFields mMagicFields;
    //Activatables mActivatables;

    //Block settings
    float mBlockDimension;
};

Dungeon& World::getDungeon(){return mDungeon;}

inline float World::getBlockDimension(){return mBlockDimension;}
inline void World::setBlockDimension( float d ){mBlockDimension = d;}

#endif