#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "Random.h"
#include "Level.h"

#define ROOM_MAX_DIMENSION 16

class WorldGenerator{
public:

    struct IRange{
        int min;
        int max;

        int genValue( Random random ){
            return random.gen( min, max + 1 );
        }
    };

    struct FRange{
        float min;
        float max;

        float genValue( Random random ){
            return ( random.genNorm() * ( max - min ) ) + min;
        }
    };

    struct LevelPreset{
        IRange roomCount;

        IRange roomCeilingHeight;

        FRange doorScrubChance;

        float roomChances[ 8 ];
    };

    struct RoomPreset{
        IRange floorSectionArea; //Sections of floors that get generated at different heights,
        FRange rampDensity;
        FRange wallDensity;
        IRange wallLength;

        struct Type{
            IRange dimensions;
            FRange furnitureDensity;
            float furnitureChances[ LEVEL_FURNITURE_TYPE_COUNT ];
        };

        Type roomTypes[ 8 ];
    };

    struct Room{

        enum Type{
            Empty,
            Labyrinth,
            BedRoom,
            Study,
            Library,
            Storage,
            DiningRoom,
            Ballroom
        };

        Type type;

        int left;
        int right;
        int top;
        int bottom;
    };

    WorldGenerator();
    ~WorldGenerator();

    //Generate a room's exits and floor layout
    void genLevel( Level& level, LevelPreset& preset );

    inline void setTileIDMax( uint tileIDMax );

protected:

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
    void genLevelRoomFurniture( Level& level, Room& room );

    void genDoors( Level& level, Room& room, Room& prevRoom, WallSide attached );
    void scrubLevelDoorways( Level& level, LevelPreset& preset );
    int applyLevelDoorways( Level& level );

    void genRoom( WallSide side, int attachX, int attachY, Room& room, LevelPreset& preset );

    //Generate furniture in a room, set px and py to where they were generated
    void genFurnitureInRoom( Level::Furniture::Type type, Level& level, Room& room, bool againstWall, int& px, int& py );
    
    void genChairByFurniture( Level::Furniture::Type type, Level& level, Room& room, int px, int py );

protected:

    Random mRand;

    RoomPreset mRoomPreset;

    Room* mRooms;
    int mRoomCount;

    Door* mDoors;
    int mDoorCount;

    uint mTileIDMax;
};

inline void WorldGenerator::setTileIDMax( uint tileIDMax ){mTileIDMax = tileIDMax;}

#endif