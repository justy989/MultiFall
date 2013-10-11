#ifndef PARTY_H
#define PARTY_H

#include "EventManager.h"
#include "Member.h"

#include <assert.h>

#define PARTY_SIZE 4
#define PARTY_LEADER_INDEX 0

class Party : public EventHandler{
public:

    Party();

    virtual void handleEvent( Event& e );

    void create( );
    void join( uint myIndex );
    void disband();

    void update( float dt );

    void memberJoin( char* name, uint index, bool ready );
    void memberLeave( uint index );
    void memberUpdateSnapshot( uint index, XMFLOAT2& snapshotPos );

    inline bool isLeader();

    inline uint getMyIndex();

    inline Member& getMember( uint index );

protected:

    bool mIsLeader;

    Member mMembers[ PARTY_SIZE ];

    uint mMyIndex;
};

inline bool Party::isLeader(){return mIsLeader;}
inline Member& Party::getMember( uint index ){assert( index < PARTY_SIZE );return mMembers[index];}
inline uint Party::getMyIndex(){return mMyIndex;}
#endif