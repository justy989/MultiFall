#ifndef NET_PACKET_H
#define NET_PACKET_H

#include "Event.h"
#include "Party.h"

struct NetPacket{
    enum Type{
        PartyJoin,
        PartyLeave,
        PartySync,
        WorldSync,
        WorldEvent
    };

    struct PartyJoinInfo{
        char name[32];

        //Info about your character
    };

    struct PartyLeaveInfo{
        uint index;
    };

    struct PartySyncInfo{
        char names[PARTY_SIZE][32];
        uint myIndex;

        //Info about each character
    };

    struct WorldSyncInfo{
        uint levelSeed;
        float pX[PARTY_SIZE];
        float pZ[PARTY_SIZE];
    };

    Type type;

    union{
        PartyJoinInfo partyJoinInfo;
        PartyLeaveInfo partyLeaveInfo;
        PartySyncInfo partySyncInfo;
        WorldSyncInfo worldSyncInfo;
        Event worldEventInfo;
    };
};

#endif