#ifndef WORLD_H
#define WORLD_H

#include "Level.h"
#include "Population.h"
#include "EventManager.h"

class World : public EventHandler{
public:

    World();

    void moveEntity( WorldEntity* entity, XMVECTOR moveVec, float dist );

    void update( float dt );

    virtual void handleEvent( Event& e );

    //Get the world's dungeon
    inline Level& getLevel();
    inline Population& getPopulation();

    //Get the block dimensions
    inline float getBlockDimension();

    //Set the block dimensions
    inline void setBlockDimension( float d );

protected:

    bool checkEntityCollision( WorldEntity* entity, XMVECTOR desiredPosition, XMVECTOR* outWallVector);

    Level mLevel;
    Population mPop;
    //Projectiles mProjs;
    //MagicFields mMagicFields;
    //Activatables mActivatables;

    //Block settings
    float mBlockDimension;
};

Level& World::getLevel(){return mLevel;}
inline Population& World::getPopulation(){return mPop;}

inline float World::getBlockDimension(){return mBlockDimension;}
inline void World::setBlockDimension( float d ){mBlockDimension = d;}

#endif