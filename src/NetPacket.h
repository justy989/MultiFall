#ifndef NETPACKET_H
#define NETPACKET_H

#include "Event.h"
#include "Party.h"

struct NetPacket{
    enum Type{
        JoinParty,
        LeaveParty,
        PartySync,
        WorldSync,
        WorldEvent
    };

    struct JoinPartyInfo{
        char name[32];

        //Info about your character
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
        JoinPartyInfo joinPartyInfo;
        PartySyncInfo partySyncInfo;
        WorldSyncInfo worldSyncInfo;
        Event worldEventInfo;
    };
};

#endif