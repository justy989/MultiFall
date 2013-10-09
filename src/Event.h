#ifndef APP_EVENT_H
#define APP_EVENT_H

#include "Utils.h"

struct Event{
    enum Type{
        //Game Events
        GenerateLevel,

        //Network Events
        NetworkTimeout,

        //Party
        PartyChat,
        PartyJoin,
        PartyLeave,

        //World Events
        CharacterSpawn,
        CharacterKill,
        CharacterWalk,
    };

    struct GenerateLevelInfo{
        uint seed;
    };

    struct PartyJoinInfo{
        uint index;
        char name[32];
    };

    struct PartyLeaveInfo{
        uint index;
    };

    struct PartyChatInfo{
        uint index;
        char message[256];
    };

    struct CharacterSpawnInfo{
        uint id;
        float x;
        float z;
    };

    struct CharacterKillInfo{
        uint id;
    };

    struct CharacterWalkInfo{
        uint id;
        float dx; //X Direction
        float dz; //Z Direction
    };

    Type type;

    union{
        PartyJoinInfo partyJoinInfo;
        PartyLeaveInfo partyLeaveInfo;
        PartyChatInfo partyChatInfo;
        GenerateLevelInfo generateLevelInfo;
        CharacterSpawnInfo characterSpawnInfo;
        CharacterKillInfo characterKillInfo;
        CharacterWalkInfo characterWalkInfo;
    };
};

#endif