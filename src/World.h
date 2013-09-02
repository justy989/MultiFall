#ifndef WORLD_H
#define WORLD_H

#include "Environment.h"

class World{
public:

    World();

    inline Environment& getEnv();

protected:

    Environment mEnv;
    //Population mPop;
};

Environment& World::getEnv(){return mEnv;}

#endif