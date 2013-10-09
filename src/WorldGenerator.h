#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "Random.h"
#include "Level.h"
#include "Population.h"

#define ROOM_MAX_DIMENSION 16
#define ROOM_TYPE_COUNT 8
#define ROOM_MAX_DOORS 4
#define ROOM_DEFAULT_HEIGHT 6
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

        int gen( Random& random ){
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

        float gen( Random& random ){
            return ( random.genNorm() * ( max - min ) ) + min;
        }
    };

    //Holding room options
    struct RoomGenerationRanges{
        IRange dimensions; //Dimensions of the room

        IRange floorSectionArea; //Sections of floors that get generated at different heights
        IRange floorHeight; //Height the floor sections will be generated at

        FRange rampDensity; //Percent chance of ramps spawning where a ramp can exist

        FRange wallDensity; //Percent of the room taken up by walls
        IRange wallLength; //Range of wall lengths

        FRange lightDensity; //Percent of the room is illuminated with light
        float lightChances[ LEVEL_LIGHT_TYPE_COUNT ]; //Chances for each light

        FRange furnitureDensity; //Percent of the room taken up by furniture
        float furnitureChances[ LEVEL_FURNITURE_TYPE_COUNT ]; //Chances for the different furniture to be generated
    };

    //Level Options
    struct LevelGenerationRanges{
        IRange roomCount; //Number of rooms to generate 
        IRange roomCeilingHeight; //Height of the ceiling

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

    struct PopulationGenerationRanges{
        FRange density;
        float enemyIDChance[ 1 ];
    };

    WorldGenerator();
    ~WorldGenerator();

    //Generate a room's exits and floor layout
    void genLevel( Level& level, LevelGenerationRanges& ranges, uint seed, float blockDimension, XMFLOAT3& spawn );
    void genPopulation( Population& population, Level& level, PopulationGenerationRanges& ranges, float blockDimension );

    inline void setTileIDMax( uint tileIDMax );

protected:

    //Wall Side structure
    enum WallSide{
        Front,
        Left,
        Back,
        Right
    };

    /* Generate a Level */

    //Generate room layout
    void genLevelBlueprint( Level& level, Room* rooms, short roomCount ); 

    //Generate the heights of the floors in each room
    void genLevelRoomHeights( Level& level, Room& room );

    //Do a pass generating the walls of each room
    void genLevelRoomWalls( Level& level, Room& room );

    //Do a pass generating the furniture of each room
    void genLevelRoomFurniture( Level& level, Room& room, float blockDimension );

    //Do a pass generating the lights of each room
    void genLevelRoomLights( Level& level, Room& room );

    //Do a pass generating the doors of each room
    void genDoors( Room* allRooms, int currentGeneratedRoomCount,
                   Room& room, Room& prevRoom, WallSide attached );

    //Do a pass scrubing doors based on a chance
    void scrubLevelDoorways( Level& level, Room* rooms, short roomCount );

    //Generate a room's dimensions alongsize an attached wall
    void genRoom( WallSide side, int attachX, int attachY, Room& room );

    //Determine whether there is a path to all doors or not
    bool pathExistsToDoors( Level& level, Room& room );
    
    //Generate chairs by furniture, return the number of indices taken up by it
    int genChairsByFurniture( Level& level, Room& room, Level::Furniture& furniture,
                              short leftIndex, short frontIndex, short rightIndex, short backIndex);

    /* Generate a Level */

protected:

    Random mRand;

    LevelGenerationRanges* mLevelRanges;

    uint mTileIDMax;
};

inline void WorldGenerator::setTileIDMax( uint tileIDMax ){mTileIDMax = tileIDMax;}

#endif