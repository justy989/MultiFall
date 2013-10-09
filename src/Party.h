#ifndef PARTY_H
#define PARTY_H

#include "EventManager.h"
#include "Member.h"

#define PARTY_SIZE 4

class Party : public EventHandler{
public:

    Party();

    void create( );
    void join( uint myIndex );

    void memberJoin( char* name, uint index );
    void memberLeave( uint index );

    inline bool isLeader();

protected:

    bool mIsLeader;

    Member mMembers[ PARTY_SIZE ];

    uint mMyIndex;
};

#endif