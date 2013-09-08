#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "Random.h"
#include "Dungeon.h"

class WorldGenerator{
public:

    struct IRange{
        int min;
        int max;
    };

    struct FRange{
        float min;
        float max;
    };

    struct LevelPreset{
        IRange roomCount;

        IRange roomWidth;
        IRange roomDepth;

        IRange roomCeilingHeight;

        FRange doorScrubChance;
    };

    struct RoomPreset{
        IRange floorSectionArea; //Sections of floors that get generated at different heights,
        FRange rampDensity;
        FRange wallDensity;
        IRange wallLength;
    };

    WorldGenerator();
    ~WorldGenerator();

    //Generate a room's exits and floor layout
    void genLevel( Level& level, LevelPreset& preset );

protected:

    struct Room{
        int left;
        int right;
        int top;
        int bottom;
    };

    struct Door{
        int x;
        int y;
        bool essential;
    };

    enum WallSide{
        Front,
        Left,
        Back,
        Right
    };

    void genLevelLayout( Level& level, LevelPreset& preset ); 
    void genLevelRoomHeights( Level& level, LevelPreset& preset, Room& room );
    void genLevelRoomWalls( Level& level, LevelPreset& preset, Room& room );

    void genDoors( Level& level, Room& room, Room& prevRoom, WallSide attached );
    void scrubLevelDoorways( Level& level, LevelPreset& preset );
    int applyLevelDoorways( Level& level );

    void genRoom( WallSide side, int attachX, int attachY, Room& room, LevelPreset& preset ); 

protected:

    Random mRand;

    RoomPreset mRoomPreset;

    Room* mRooms;
    int mRoomCount;

    Door* mDoors;
    int mDoorCount;
};

#endif