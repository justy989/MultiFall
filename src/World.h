#ifndef WORLD_H
#define WORLD_H

#include "Level.h"
#include "Population.h"
#include "EventManager.h"
#include "ItemMaster.h"

class World : public EventHandler{
public:

    World();

    void moveEntity( WorldEntity* entity, XMVECTOR moveVec, float dist );

    void update( float dt );

    virtual void handleEvent( Event& e );

    //Get the world's dungeon
    inline Level& getLevel();
    inline Population& getPopulation();
    inline ItemMaster& getItemMaster();

    //Get the block dimensions
    inline float getBlockDimension();

    //Set the block dimensions
    inline void setBlockDimension( float d );

    inline WorldContainer* getOpenedContainer();

protected:

    bool checkEntityCollision( WorldEntity* entity, XMVECTOR desiredPosition, XMVECTOR* outWallVector);

    WorldContainer* findCloseContainer( uint memberIndex );

protected:

    Level mLevel;
    Population mPop;
    //Projectiles mProjs;
    //MagicFields mMagicFields;
    //Activatables mActivatables;

    ItemMaster mItemMaster;

    WorldContainer* mOpenedContainer;

    //Block settings
    float mBlockDimension;
};

inline Level& World::getLevel(){return mLevel;}
inline Population& World::getPopulation(){return mPop;}
inline ItemMaster& World::getItemMaster(){return mItemMaster;}

inline float World::getBlockDimension(){return mBlockDimension;}
inline void World::setBlockDimension( float d ){mBlockDimension = d;}

inline WorldContainer* World::getOpenedContainer(){return mOpenedContainer;}

#endif