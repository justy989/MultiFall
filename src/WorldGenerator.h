#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "Random.h"
#include "Environment.h"

class WorldGenerator{
public:

    WorldGenerator();

    //Generate a room's exits and floor layout
    void genRoom( Environment::Room& room );

protected:

    Random mRand;
};

#endif