#ifndef APP_EVENT_H
#define APP_EVENT_H

#include "Utils.h"

struct Event{

    Event() : clientGenerated(true), type(InvalidEvent) {}

    enum Type{
        InvalidEvent,

        //Network Events
        NetworkListen,      //Attempt to listen
        NetworkIsListening, //Successfully listening
        NetworkConnect,     //Attempt to connect
        NetworkIsConnected, //Successfully connected
        NetworkTimeout,     //User timed out
        NetworkDisconnect,  //Disconnect the socket

        //Party
        PartyChat,           //chat message
        PartyMemberJoin,     //A party member joined
        PartyMemberLeave,    //A party member left
        PartyMemberReady,    //A party member is ready to start
        PartyMemberUnReady,  //A party member is not ready to start
        PartyMemberKick,     //A party member was kicked by the leader
        PartyJoinRequest,    //Request to join the party
        PartyJoinAccept,     //Accepted into the party
        PartyJoinRejectFull, //Rejected due to the party being full
        PartyJoinRejectInSession, //Rejected due to the party being in a session already
        PartyDisband,        //Party has disbanded
        PartyMemberCharacterUpdate, //Update of the character info for a party member, such as pos, facingDirection, current action

        //Session Packets
        SessionStart,    //Start the session
        SessionPause,    //Pause session for all players
        SessionComplete, //Session is complete, back to lobby

        //World Events
        CharacterSpawn, //Spawn a character by id and location
        CharacterKill,  //Kill a character by id
        CharacterWalk,  //move a character in a direction

        DoorOpen, //Open the specified door
        DoorClose, //Close the specified door

        ContainerOpen,
        ContainerClose
    };

    /**********************************************************************************
    NOTICE: These are copied often, make sure there aren't pointers to allocated memory
            make sure there is a buffer so the = operator works properly
    ***********************************************************************************/

    struct NetworkListenInfo{
        ushort port;
    };

    struct NetworkConnectInfo{
        char host[ 128 ];
        ushort port;
    };

    struct PartyChatInfo{
        uint memberIndex;
        char message[ CHAT_MESSAGE_LEN ];
    };

    struct PartyMemberJoinInfo{
        char name[ NAME_LEN ];
        uint memberIndex;
        bool ready;
    };

    struct PartyMemberLeaveInfo{
        uint memberIndex;
    };

    struct PartyMemberReadyInfo{
        uint memberIndex;
    };

    struct PartyMemberUnReadyInfo{
        uint memberIndex;
    };

    struct PartyMemberKickInfo{
        uint memberIndex;
    };

    struct PartyJoinRequestInfo{
        char name[ NAME_LEN ];
    };

    struct PartyJoinAcceptInfo{
        uint memberIndex;
    };

    struct PartyMemberCharacterUpdateInfo{
        float x; //X position 
        float z; //Y position
        float fx; //facing X position
        float fz; //facing Y position
        uint memberIndex; //index of the member
    };

    struct SessionStartInfo{
        uint levelSeed;
        //float pX[ PARTY_SIZE ];
        //float pZ[ PARTY_SIZE ];
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

    struct DoorOpenInfo{
        uint id;
    };

    struct DoorCloseInfo{
        uint id;
    };

    struct ContainerOpenInfo{
        uint memberIndex;
    };

    struct ContainerCloseInfo{
        uint memberIndex;
    };

    Type type; //The type of event it is
    bool clientGenerated; //Whether this is generated by the running client or not

    //The arguments for each event type
    union{
        NetworkListenInfo networkListenInfo;
        NetworkConnectInfo networkConnectInfo;

        PartyChatInfo partyChatInfo;
        PartyMemberJoinInfo partyMemberJoinInfo;
        PartyMemberLeaveInfo partyMemberLeaveInfo;
        PartyMemberReadyInfo partyMemberReadyInfo;
        PartyMemberUnReadyInfo partyMemberUnReadyInfo;
        PartyMemberKickInfo partyMemberKickInfo;
        PartyJoinRequestInfo partyJoinRequestInfo;
        PartyJoinAcceptInfo partyJoinAcceptInfo;
        PartyMemberCharacterUpdateInfo partyMemberCharacterUpdateInfo;

        SessionStartInfo sessionStartInfo;

        CharacterSpawnInfo characterSpawnInfo;
        CharacterKillInfo characterKillInfo;
        CharacterWalkInfo characterWalkInfo;

        DoorOpenInfo doorOpenInfo;
        DoorCloseInfo doorCloseInfo;

        ContainerOpenInfo containerOpenInfo;
        ContainerCloseInfo containerCloseInfo;
    };
};

#endif