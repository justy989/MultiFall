#ifndef APP_EVENT_H
#define APP_EVENT_H

#include "Utils.h"

struct Event{
    enum Type{
        //Game Events
        GenerateLevel,

        //Network Events

        //World Events
        CharacterSpawn,
        CharacterKill,
        CharacterWalk,
    };

    struct GenerateLevelInfo{
        uint seed;
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
        GenerateLevelInfo generateLevelInfo;
        CharacterSpawnInfo characterSpawnInfo;
        CharacterKillInfo characterKillInfo;
        CharacterWalkInfo characterWalkInfo;
    };
};

#endif