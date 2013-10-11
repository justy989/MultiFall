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
        mMembers[i].setReady( false );
    }
}

void Party::join( uint myIndex )
{
    assert( myIndex < PARTY_SIZE );

    mMyIndex = myIndex;
    mIsLeader = false;
    mMembers[ mMyIndex ].setExist( true );
}

void Party::disband()
{
    for(int i = 0; i < PARTY_SIZE; i++){
        mMembers[i].setExist( false );
        mMembers[i].setReady( false );
    }

    mIsLeader = false;
}

void Party::memberJoin( char* name, uint index, bool ready )
{
    assert( index < PARTY_SIZE );

    mMembers[ index ].setName( name );
    mMembers[ index ].setExist( true );
    mMembers[ index ].setReady( ready );
}

void Party::memberLeave( uint index )
{
    assert( index < PARTY_SIZE );

    mMembers[ index ].setExist( false );
}

void Party::handleEvent( Event& e )
{
    switch( e.type ){
    case Event::Type::PartyJoinAccept:
        join( e.partyJoinAcceptInfo.userIndex );
        break;
    case Event::Type::PartyMemberReady:
        assert( e.partyMemberReadyInfo.userIndex < PARTY_SIZE );
        mMembers[ e.partyMemberReadyInfo.userIndex ].setReady( true );
        break;
    case Event::Type::PartyMemberUnReady:
        assert( e.partyMemberReadyInfo.userIndex < PARTY_SIZE );
        mMembers[ e.partyMemberReadyInfo.userIndex ].setReady( false );
        break;
    case Event::Type::PartyMemberJoin:
        memberJoin( e.partyMemberJoinInfo.name, e.partyMemberJoinInfo.userIndex, e.partyMemberJoinInfo.ready );
        LOG_INFO << mMembers[ e.partyMemberJoinInfo.userIndex ].getName() << " joined the party." << LOG_ENDL;
        break;
    case Event::Type::PartyMemberLeave:
        memberLeave( e.partyMemberLeaveInfo.userIndex );
        LOG_INFO << mMembers[ e.partyMemberLeaveInfo.userIndex ].getName() << " left the party." << LOG_ENDL;
        break;
    case Event::Type::PartyDisband:
        disband();
        LOG_INFO << "Party Disbanded." << LOG_ENDL;
        break;
    case Event::Type::PartyChat:
        if( e.partyChatInfo.userIndex == PARTY_LEADER_INDEX ){
            LOG_INFO << "[L] " << mMembers[ e.partyChatInfo.userIndex ].getName() << " : " << LOG_ENDL;
        }else{
            LOG_INFO << mMembers[ e.partyChatInfo.userIndex ].getName() << " : " << LOG_ENDL;
        }
        break;
    default:
        break;
    }
}