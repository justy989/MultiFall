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
    mMyIndex = PARTY_SIZE;
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

void Party::update( float dt )
{
    for(int i = 0; i < PARTY_SIZE; i++){
        if( mMembers[i].doesExist() ){
            mMembers[i].update(dt);
        }
    }
}

void Party::memberUpdateSnapshot( uint index, XMFLOAT2& snapshotPos )
{
    assert( index < PARTY_SIZE );

    mMembers[ index ].updateSnapshot( snapshotPos );
}

void Party::handleEvent( Event& e )
{
    switch( e.type ){
    case Event::Type::PartyJoinAccept:
        join( e.partyJoinAcceptInfo.memberIndex );
        break;
    case Event::Type::PartyMemberReady:
        assert( e.partyMemberReadyInfo.memberIndex < PARTY_SIZE );
        mMembers[ e.partyMemberReadyInfo.memberIndex ].setReady( true );
        break;
    case Event::Type::PartyMemberUnReady:
        assert( e.partyMemberReadyInfo.memberIndex < PARTY_SIZE );
        mMembers[ e.partyMemberReadyInfo.memberIndex ].setReady( false );
        break;
    case Event::Type::PartyMemberJoin:
        memberJoin( e.partyMemberJoinInfo.name, e.partyMemberJoinInfo.memberIndex, e.partyMemberJoinInfo.ready );
        LOG_INFO << mMembers[ e.partyMemberJoinInfo.memberIndex ].getName() << " joined the party." << LOG_ENDL;
        break;
    case Event::Type::PartyMemberLeave:
        memberLeave( e.partyMemberLeaveInfo.memberIndex );
        LOG_INFO << mMembers[ e.partyMemberLeaveInfo.memberIndex ].getName() << " left the party." << LOG_ENDL;
        break;
    case Event::Type::PartyMemberCharacterUpdate:
        memberUpdateSnapshot( e.partyMemberCharacterUpdateInfo.memberIndex, 
                              XMFLOAT2( e.partyMemberCharacterUpdateInfo.x, e.partyMemberCharacterUpdateInfo.z ) );
        break;
    case Event::Type::PartyDisband:
        disband();
        LOG_INFO << "Party Disbanded." << LOG_ENDL;
        break;
    case Event::Type::PartyChat:
        if( e.partyChatInfo.memberIndex == PARTY_LEADER_INDEX ){
            LOG_INFO << "[L] " << mMembers[ e.partyChatInfo.memberIndex ].getName() << " : " << LOG_ENDL;
        }else{
            LOG_INFO << mMembers[ e.partyChatInfo.memberIndex ].getName() << " : " << LOG_ENDL;
        }
        break;
    default:
        break;
    }
}