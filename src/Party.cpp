#include "Party.h"

#include <assert.h>

Party::Party() :
    mIsLeader(false),
    mMyIndex( PARTY_SIZE )
{

}

void Party::create( )
{
    mIsLeader = true;
    mMyIndex = 0;

    for(int i = 0; i < PARTY_SIZE; i++){
        mMembers[i].setExist( i == mMyIndex );
    }
}

void Party::join( uint myIndex )
{
    assert( myIndex < PARTY_SIZE );

    mMyIndex = myIndex;
    mIsLeader = false;
}

void Party::memberJoin( char* name, uint index )
{
    assert( index < PARTY_SIZE );

    mMembers[ index ].setName( name );
    mMembers[ index ].setExist( true );
}

void Party::memberLeave( uint index )
{
    assert( index < PARTY_SIZE );

    mMembers[ index ].setExist( false );
}