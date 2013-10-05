#include "Population.h"

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
    for(int i = 0; i < POPULATION_MAX_CHARACTERS; i++){
        if( mCharacters[i].getExistence() == WorldEntity::Existence::Dead ){
            mCharacters[i].setExistence( WorldEntity::Existence::Alive );
            mCharacters[i].getPosition() = position;
            mCharacters[i].setID( id );
            return i;
        }
    }

    return POPULATION_MAX_CHARACTERS;
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