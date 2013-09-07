#ifndef DUNGEON_H
#define DUNGEON_H

#include "Level.h"
#include "Utils.h"

class Dungeon{
public:

    Dungeon();

    inline Level& getLevel();

protected:

    Level mLevel;

    //Lights

    //Furniture
};

inline Level& Dungeon::getLevel(){return mLevel;}

#endif