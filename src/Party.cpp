#include "Party.h"
#include "Log.h"

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

void Party::disband()
{
    for(int i = 0; i < PARTY_SIZE; i++){
        mMembers[i].setExist( false );
    }

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

void Party::handleEvent( Event& e )
{
    switch( e.type ){
    case Event::Type::PartyJoin:
        memberJoin( e.partyJoinInfo.name, e.partyJoinInfo.index );
        LOG_INFO << mMembers[ e.partyJoinInfo.index ].getName() << " joined the party." << LOG_ENDL;
        break;
    case Event::Type::PartyLeave:
        memberLeave( e.partyLeaveInfo.index );
        LOG_INFO << mMembers[ e.partyChatInfo.index ].getName() << " left the party." << LOG_ENDL;
        break;
    case Event::Type::PartyDisband:
        disband();
        LOG_INFO << "Party Disbanded." << LOG_ENDL;
        break;
    case Event::Type::PartyChat:
        if( e.partyChatInfo.index == PARTY_LEADER_INDEX ){
            LOG_INFO << "[L] " << mMembers[ e.partyChatInfo.index ].getName() << " : " << LOG_ENDL;
        }else{
            LOG_INFO << mMembers[ e.partyChatInfo.index ].getName() << " : " << LOG_ENDL;
        }
        break;
    default:
        break;
    }
}