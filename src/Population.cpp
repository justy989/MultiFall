#include "Population.h"

#include "Party.h"

#include <fstream>
#include <assert.h>

Population::Population()
{
    clear();
}

void Population::update( float dt )
{
    for(int i = 0; i < POPULATION_MAX_CHARACTERS; i++){
        if( mCharacters[i].getExistence() != WorldEntity::Existence::Dead ){
            mCharacters[i].update( dt );
        }
    }
}

ushort Population::spawn( ushort id, XMFLOAT4 position )
{
    //Start at party size, in order to leave room for the party
    for(int i = PARTY_SIZE; i < POPULATION_MAX_CHARACTERS; i++){
        if( mCharacters[i].getExistence() == WorldEntity::Existence::Dead ){
            mCharacters[i].setExistence( WorldEntity::Existence::Alive );
            mCharacters[i].getPosition() = position;
            mCharacters[i].setID( id );
            mCharacters[i].setAlliance( Character::Alliance::Enemy );
            return i;
        }
    }

    return POPULATION_MAX_CHARACTERS;
}

ushort Population::spawnMember( ushort id, XMFLOAT4 position )
{
    for(int i = 0; i < PARTY_SIZE; i++){
         if( mCharacters[i].getExistence() == WorldEntity::Existence::Dead ){
            mCharacters[i].setExistence( WorldEntity::Existence::Alive );
            mCharacters[i].getPosition() = position;
            mCharacters[i].setID( id );
            mCharacters[i].setAlliance( Character::Alliance::Party );
            return i;
        }
    }

    return PARTY_SIZE;
}

void Population::kill( ushort id )
{
    if( id < POPULATION_MAX_CHARACTERS ){
        if( mCharacters[ id ].getExistence() != WorldEntity::Existence::Dead ){
            mCharacters[ id ].setExistence( WorldEntity::Existence::Dead );
        }
    }
}

Character& Population::getCharacter( ushort index )
{
    assert( index < POPULATION_MAX_CHARACTERS );
    return mCharacters[ index ];
}

void Population::clear()
{
    for(int i = 0; i < POPULATION_MAX_CHARACTERS; i++){
        mCharacters[i].setExistence( WorldEntity::Existence::Dead );
    }
}

Population::EnemyDefinition Population::getDefinitionFromMasterList( uint id )
{
    EnemyDefinition stats;

    return stats;
}