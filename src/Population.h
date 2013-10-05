#ifndef POPULATION_H
#define POPULATION_H

#include "Character.h"

#define POPULATION_MAX_CHARACTERS 256

class Population{
public:

    //Constr
    Population();

    //Update characters over time
    void update( float dt );

    //Spawn an entity, return index 
    ushort spawn( ushort id, XMFLOAT4 position );

    //Get a character from the population
    Character& getCharacter( ushort index );

protected:

    Character mCharacters[ POPULATION_MAX_CHARACTERS ];
};

#endif