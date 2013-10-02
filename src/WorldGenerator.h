#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "Random.h"
#include "Level.h"

#define ROOM_MAX_DIMENSION 16
#define ROOM_TYPE_COUNT 8
#define ROOM_MAX_DOORS 4
#define ROOM_DEFAULT_HEIGHT 8
#define WORLD_GEN_ATTEMPTS 256

class WorldGenerator{
public:

    //Integer range
    struct IRange{
        int min;
        int max;

        void set( int min, int max ){
            this->min = min;
            this->max = max;
        }

        int gen( Random random ){
            return random.gen( min, max + 1 );
        }
    };

    //Float range
    struct FRange{
        float min;
        float max;

        void set( float min, float max ){
            this->min = min;
            this->max = max;
        }

        float gen( Random random ){
            return ( random.genNorm() * ( max - min ) ) + min;
        }
    };

    //Holding room options
    struct RoomGenerationRanges{
        IRange dimensions; //Dimensions of the room

        IRange floorSectionArea; //Sections of floors that get generated at different heights
        IRange floorHeight; //Height the floor sections will be generated at
        
        IRange ceilingHeight; //Height of the ceiling

        FRange rampDensity; //Percent chance of ramps spawning where a ramp can exist

        FRange wallDensity; //Percent of the room taken up by walls
        IRange wallLength; //Range of wall lengths

        FRange furnitureDensity; //Percent of the room taken up by furniture
        float furnitureChances[ LEVEL_FURNITURE_TYPE_COUNT ]; //Chances for the different furniture to be generated
    };

    //Level Options
    struct LevelGenerationRanges{
        IRange roomCount; //Number of rooms to generate

        FRange doorScrubChance; //Chance to scrub unnecessary doors

        RoomGenerationRanges rooms[ ROOM_TYPE_COUNT ]; //Room Generation ranges for each room type
        float roomChances[ ROOM_TYPE_COUNT ]; //Chances for each room to be spawned
    };

    //Structure for holding a door as part of a room
    struct Door{

        Door() : x(-1), y(-1), essential(false){}

        int x;
        int y;
        bool essential;
    };

    //Structure holding a room 
    struct Room{
        //Types of rooms we can generate
        enum Type{
            Empty,
            Labyrinth,
            BedRoom,
            Study,
            Library,
            Storage,
            DiningRoom,
            BallRoom
        };

        Room() : type(Empty), 
            left(0), right(0),
            top(0), bottom(0) {}

        Type type;

        //Room bounds
        int left;
        int right;
        int top;
        int bottom;

        Door doors[ ROOM_MAX_DOORS ]; //One for each side
    };

    WorldGenerator();
    ~WorldGenerator();

    //Generate a room's exits and floor layout
    void genLevel( Level& level, LevelGenerationRanges& ranges );

    inline void setTileIDMax( uint tileIDMax );

protected:

    //Wall Side structure
    enum WallSide{
        Front,
        Left,
        Back,
        Right
    };

    //Generated Furniture structure 
    struct GeneratedFurniture{
        //Inclusive index box where furniture will be generated
        ushort leftRoomIndex;
        ushort topRoomIndex;
        ushort rightRoomIndex;
        ushort bottomRoomIndex;

        Level::Furniture furniture;
    };

    /* Generate a Level */

    //Generate room layout
    void genLevelBlueprint( Level& level, Room* rooms, short roomCount ); 

    //Generate the heights of the floors in each room
    void genLevelRoomHeights( Level& level, Room& room );

    //Do a pass generating the walls of each room
    void genLevelRoomWalls( Level& level, Room& room );

    //Do a pass generating the furniture of each room
    void genLevelRoomFurniture( Level& level, Room& room );

    //Do a pass generating the doors of each room
    void genDoors( Level& level, Room& room, Room& prevRoom, WallSide attached );

    //Do a pass scrubing doors based on a chance
    void scrubLevelDoorways( Level& level, Room* rooms, short roomCount );

    //Generate a room's dimensions alongsize an attached wall
    void genRoom( WallSide side, int attachX, int attachY, Room& room );

    //Generate a piece of furniture in a room
    void genFurnitureInRoom( Level& level, Room& room, Level::Furniture::Type type, bool againstWall, GeneratedFurniture& gennedFurniture );

    //Determine whether there is a path to all doors or not
    bool pathExistsToDoors( Level& level, Room& room );
    
    //void genChairByFurniture( Level& level, Room& room, Level::Furniture::Type type, GeneratedFurniture& gennedFurniture, GeneratedFurniture& gennedChair );

    /* Generate a Level */

protected:

    Random mRand;

    LevelGenerationRanges* mLevelRanges;

    uint mTileIDMax;
};

inline void WorldGenerator::setTileIDMax( uint tileIDMax ){mTileIDMax = tileIDMax;}

#endif