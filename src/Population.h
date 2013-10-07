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

    //Kill a character
    void kill( ushort id );

    //Get a character from the population
    Character& getCharacter( ushort index );

    //Clear the population... mass genocide, nbd
    void clear();

protected:

    Character mCharacters[ POPULATION_MAX_CHARACTERS ];
};

#endif