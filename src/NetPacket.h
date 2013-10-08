#ifndef NETPACKET_H
#define NETPACKET_H

#include "Event.h"

struct NetPacket{
    enum Type{
        User,
        Party,
        Sync,
        WorldEvent
    };

    struct UserInfo{
        char name[32];
    };

    struct PartyInfo{
        UserInfo users[4];
        uint userIndex;
    };

    struct SyncInfo{
        uint levelSeed;
        float pX[4];
        float pZ[4];
    };

    Type type;

    union{
        UserInfo userInfo;
        PartyInfo partyInfo;
        SyncInfo syncInfo;
        Event worldEventInfo;
    };
};

#endif