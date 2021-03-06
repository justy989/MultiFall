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

    //Spawn a party member in one of the first indices
    ushort spawnMember( ushort id, XMFLOAT4 position );

    //Kill a character
    void kill( ushort id );

    //Get a character from the population
    Character& getCharacter( ushort index );

    //Clear the population... mass genocide, nbd
    void clear();

protected:

    struct EnemyDefinition{

        EnemyDefinition() : AIFlags(0), SpellAccessFlags(0) {}

        Stats stats;
        uint AIFlags;
        uint SpellAccessFlags;
    };

    EnemyDefinition getDefinitionFromMasterList( uint id );

protected:
    char* mEnemyMasterListPath;

    Character mCharacters[ POPULATION_MAX_CHARACTERS ];
};

#endif