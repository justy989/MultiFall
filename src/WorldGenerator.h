#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "Random.h"
#include "Dungeon.h"

class WorldGenerator{
public:

    WorldGenerator();

    //Generate a room's exits and floor layout
    void genLevel( Level& level );

protected:

    struct Room{
        int left;
        int right;
        int top;
        int bottom;
    };

    void genLevelLayout( Level& level ); 
    void genLevelRooms( Level& level, Room& rooms );
    void genLevelDoorways( Level& level );

protected:

    Random mRand;
};

#endif