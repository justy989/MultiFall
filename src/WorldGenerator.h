#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "Random.h"
#include "Environment.h"

class WorldGenerator{
public:

    WorldGenerator();

    void genRoom( Environment::Room& room );

protected:
    Random mRand;
};

#endif