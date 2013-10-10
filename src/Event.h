#ifndef APP_EVENT_H
#define APP_EVENT_H

#include "Utils.h"

struct Event{
    enum Type{
        //Network Events
        NetworkListen,
        NetworkIsListening,
        NetworkConnect,
        NetworkIsConnected,
        NetworkTimeout, //User timed out
        NetworkDisconnect,

        //Party
        PartyChat,    //Chat messages between party members
        PartyJoin,    //Someone has joined the party
        PartyLeave,   //Someone has left the party
        PartyDisband, //Party no longer exists

        //World Events
        CharacterSpawn, //Spawn a character by id and location
        CharacterKill, //Kill a character by id
        CharacterWalk, //move a character in a direction
    };

    struct NetworkListenInfo{
        ushort port;
    };

    struct NetworkConnectInfo{
        char* host;
        ushort port;
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
        NetworkListenInfo networkListenInfo;
        NetworkConnectInfo networkConnectInfo;

        PartyJoinInfo partyJoinInfo;
        PartyLeaveInfo partyLeaveInfo;
        PartyChatInfo partyChatInfo;

        CharacterSpawnInfo characterSpawnInfo;
        CharacterKillInfo characterKillInfo;
        CharacterWalkInfo characterWalkInfo;
    };
};

#endif