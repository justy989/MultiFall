#include "WorldGenerator.h"
#include "Utils.h"
#include "Log.h"

#include <time.h>

WorldGenerator::WorldGenerator() : 
    mRooms(NULL),
    mRoomCount(0),
    mTileIDMax(16)
{
    mRand.seed( time( 0 ) );

    mRoomPreset.floorSectionArea.min = 200;
    mRoomPreset.floorSectionArea.max = 300;

    mRoomPreset.rampDensity.min = 0.0f;
    mRoomPreset.rampDensity.max = 1.0f;

    mRoomPreset.wallDensity.min = 0.0f;
    mRoomPreset.wallDensity.max = 0.1f;

    mRoomPreset.wallLength.min = 1;
    mRoomPreset.wallLength.max = 16;

    //Empty Room presets
    RoomPreset::Type& empty = mRoomPreset.mRoomTypes[ Room::Type::Empty ];

    empty.width.min = 2;
    empty.width.max = 12;

    empty.height = empty.width;

    empty.furnitureCount[ Level::Furniture::Type::None ].min = 0;
    empty.furnitureCount[ Level::Furniture::Type::None ].max = 0;

    empty.furnitureChance[ Level::Furniture::Type::None ].min = 0.0f;
    empty.furnitureChance[ Level::Furniture::Type::None ].max = 0.0f;

    empty.furnitureCount[ Level::Furniture::Type::Chair ].min = 0;
    empty.furnitureCount[ Level::Furniture::Type::Chair ].max = 0;

    empty.furnitureChance[ Level::Furniture::Type::Chair ].min = 0.0f;
    empty.furnitureChance[ Level::Furniture::Type::Chair ].max = 0.0f;

    empty.furnitureCount[ Level::Furniture::Type::Desk ].min = 0;
    empty.furnitureCount[ Level::Furniture::Type::Desk ].max = 0;

    empty.furnitureChance[ Level::Furniture::Type::Desk ].min = 0.0f;
    empty.furnitureChance[ Level::Furniture::Type::Desk ].max = 0.0f;

    empty.furnitureCount[ Level::Furniture::Type::Table ].min = 0;
    empty.furnitureCount[ Level::Furniture::Type::Table ].max = 0;

    empty.furnitureChance[ Level::Furniture::Type::Table ].min = 0.0f;
    empty.furnitureChance[ Level::Furniture::Type::Table ].max = 0.0f;

    empty.furnitureCount[ Level::Furniture::Type::Bench ].min = 0;
    empty.furnitureCount[ Level::Furniture::Type::Bench ].max = 0;

    empty.furnitureChance[ Level::Furniture::Type::Bench ].min = 0.0f;
    empty.furnitureChance[ Level::Furniture::Type::Bench ].max = 0.0f;

    empty.furnitureCount[ Level::Furniture::Type::Bed_Frame ].min = 0;
    empty.furnitureCount[ Level::Furniture::Type::Bed_Frame ].max = 0;

    empty.furnitureChance[ Level::Furniture::Type::Bed_Frame ].min = 0.0f;
    empty.furnitureChance[ Level::Furniture::Type::Bed_Frame ].max = 0.0f;

    empty.furnitureCount[ Level::Furniture::Type::Book_Case ].min = 0;
    empty.furnitureCount[ Level::Furniture::Type::Book_Case ].max = 0;

    empty.furnitureChance[ Level::Furniture::Type::Book_Case ].min = 0.0f;
    empty.furnitureChance[ Level::Furniture::Type::Book_Case ].max = 0.0f;

    //Labyrinth presets
    RoomPreset::Type& labyrinth = mRoomPreset.mRoomTypes[ Room::Type::Labyrinth ];

    labyrinth.width.min = 2;
    labyrinth.width.max = 12;

    labyrinth.height = labyrinth.width;

    labyrinth.furnitureCount[ Level::Furniture::Type::None ].min = 0;
    labyrinth.furnitureCount[ Level::Furniture::Type::None ].max = 0;

    labyrinth.furnitureChance[ Level::Furniture::Type::None ].min = 0.0f;
    labyrinth.furnitureChance[ Level::Furniture::Type::None ].max = 0.0f;

    labyrinth.furnitureCount[ Level::Furniture::Type::Chair ].min = 0;
    labyrinth.furnitureCount[ Level::Furniture::Type::Chair ].max = 0;

    labyrinth.furnitureChance[ Level::Furniture::Type::Chair ].min = 0.0f;
    labyrinth.furnitureChance[ Level::Furniture::Type::Chair ].max = 0.0f;

    labyrinth.furnitureCount[ Level::Furniture::Type::Desk ].min = 0;
    labyrinth.furnitureCount[ Level::Furniture::Type::Desk ].max = 0;

    labyrinth.furnitureChance[ Level::Furniture::Type::Desk ].min = 0.0f;
    labyrinth.furnitureChance[ Level::Furniture::Type::Desk ].max = 0.0f;

    labyrinth.furnitureCount[ Level::Furniture::Type::Table ].min = 0;
    labyrinth.furnitureCount[ Level::Furniture::Type::Table ].max = 0;

    labyrinth.furnitureChance[ Level::Furniture::Type::Table ].min = 0.0f;
    labyrinth.furnitureChance[ Level::Furniture::Type::Table ].max = 0.0f;

    labyrinth.furnitureCount[ Level::Furniture::Type::Bench ].min = 0;
    labyrinth.furnitureCount[ Level::Furniture::Type::Bench ].max = 0;

    labyrinth.furnitureChance[ Level::Furniture::Type::Bench ].min = 0.0f;
    labyrinth.furnitureChance[ Level::Furniture::Type::Bench ].max = 0.0f;

    labyrinth.furnitureCount[ Level::Furniture::Type::Bed_Frame ].min = 0;
    labyrinth.furnitureCount[ Level::Furniture::Type::Bed_Frame ].max = 0;

    labyrinth.furnitureChance[ Level::Furniture::Type::Bed_Frame ].min = 0.0f;
    labyrinth.furnitureChance[ Level::Furniture::Type::Bed_Frame ].max = 0.0f;

    labyrinth.furnitureCount[ Level::Furniture::Type::Book_Case ].min = 0;
    labyrinth.furnitureCount[ Level::Furniture::Type::Book_Case ].max = 0;

    labyrinth.furnitureChance[ Level::Furniture::Type::Book_Case ].min = 0.0f;
    labyrinth.furnitureChance[ Level::Furniture::Type::Book_Case ].max = 0.0f;

    //Bedroom presets
    RoomPreset::Type& bedRoom = mRoomPreset.mRoomTypes[ Room::Type::BedRoom ];

    bedRoom.width.min = 2;
    bedRoom.width.max = 4;

    bedRoom.height = bedRoom.width;

    bedRoom.furnitureCount[ Level::Furniture::Type::None ].min = 0;
    bedRoom.furnitureCount[ Level::Furniture::Type::None ].max = 0;

    bedRoom.furnitureChance[ Level::Furniture::Type::None ].min = 0.0f;
    bedRoom.furnitureChance[ Level::Furniture::Type::None ].max = 0.0f;

    bedRoom.furnitureCount[ Level::Furniture::Type::Chair ].min = 1;
    bedRoom.furnitureCount[ Level::Furniture::Type::Chair ].max = 1;

    bedRoom.furnitureChance[ Level::Furniture::Type::Chair ].min = 0.5f;
    bedRoom.furnitureChance[ Level::Furniture::Type::Chair ].max = 0.8f;

    bedRoom.furnitureCount[ Level::Furniture::Type::Desk ].min = 1;
    bedRoom.furnitureCount[ Level::Furniture::Type::Desk ].max = 2;

    bedRoom.furnitureChance[ Level::Furniture::Type::Desk ].min = 0.25f;
    bedRoom.furnitureChance[ Level::Furniture::Type::Desk ].max = 0.5f;

    bedRoom.furnitureCount[ Level::Furniture::Type::Table ].min = 0;
    bedRoom.furnitureCount[ Level::Furniture::Type::Table ].max = 0;

    bedRoom.furnitureChance[ Level::Furniture::Type::Table ].min = 0.0f;
    bedRoom.furnitureChance[ Level::Furniture::Type::Table ].max = 0.0f;

    bedRoom.furnitureCount[ Level::Furniture::Type::Bench ].min = 0;
    bedRoom.furnitureCount[ Level::Furniture::Type::Bench ].max = 0;

    bedRoom.furnitureChance[ Level::Furniture::Type::Bench ].min = 0.0f;
    bedRoom.furnitureChance[ Level::Furniture::Type::Bench ].max = 0.0f;

    bedRoom.furnitureCount[ Level::Furniture::Type::Bed_Frame ].min = 1;
    bedRoom.furnitureCount[ Level::Furniture::Type::Bed_Frame ].max = 1;

    bedRoom.furnitureChance[ Level::Furniture::Type::Bed_Frame ].min = 1.0f;
    bedRoom.furnitureChance[ Level::Furniture::Type::Bed_Frame ].max = 1.0f;

    bedRoom.furnitureCount[ Level::Furniture::Type::Book_Case ].min = 1;
    bedRoom.furnitureCount[ Level::Furniture::Type::Book_Case ].max = 1;

    bedRoom.furnitureChance[ Level::Furniture::Type::Book_Case ].min = 0.25f;
    bedRoom.furnitureChance[ Level::Furniture::Type::Book_Case ].max = 0.5f;

    //Study presets
    RoomPreset::Type& study = mRoomPreset.mRoomTypes[ Room::Type::Study ];

    study.width.min = 3;
    study.width.max = 5;

    study.height = study.width;

    study.furnitureCount[ Level::Furniture::Type::None ].min = 0;
    study.furnitureCount[ Level::Furniture::Type::None ].max = 0;

    study.furnitureChance[ Level::Furniture::Type::None ].min = 0.0f;
    study.furnitureChance[ Level::Furniture::Type::None ].max = 0.0f;

    study.furnitureCount[ Level::Furniture::Type::Chair ].min = 1;
    study.furnitureCount[ Level::Furniture::Type::Chair ].max = 1;

    study.furnitureChance[ Level::Furniture::Type::Chair ].min = 1.0f;
    study.furnitureChance[ Level::Furniture::Type::Chair ].max = 1.0f;

    study.furnitureCount[ Level::Furniture::Type::Desk ].min = 1;
    study.furnitureCount[ Level::Furniture::Type::Desk ].max = 3;

    study.furnitureChance[ Level::Furniture::Type::Desk ].min = 1.0f;
    study.furnitureChance[ Level::Furniture::Type::Desk ].max = 1.0f;

    study.furnitureCount[ Level::Furniture::Type::Table ].min = 1;
    study.furnitureCount[ Level::Furniture::Type::Table ].max = 1;

    study.furnitureChance[ Level::Furniture::Type::Table ].min = 0.5f;
    study.furnitureChance[ Level::Furniture::Type::Table ].max = 0.5f;

    study.furnitureCount[ Level::Furniture::Type::Bench ].min = 0;
    study.furnitureCount[ Level::Furniture::Type::Bench ].max = 0;

    study.furnitureChance[ Level::Furniture::Type::Bench ].min = 0.0f;
    study.furnitureChance[ Level::Furniture::Type::Bench ].max = 0.0f;

    study.furnitureCount[ Level::Furniture::Type::Bed_Frame ].min = 0;
    study.furnitureCount[ Level::Furniture::Type::Bed_Frame ].max = 0;

    study.furnitureChance[ Level::Furniture::Type::Bed_Frame ].min = 0.0f;
    study.furnitureChance[ Level::Furniture::Type::Bed_Frame ].max = 0.0f;

    study.furnitureCount[ Level::Furniture::Type::Book_Case ].min = 1;
    study.furnitureCount[ Level::Furniture::Type::Book_Case ].max = 3;

    study.furnitureChance[ Level::Furniture::Type::Book_Case ].min = 0.85f;
    study.furnitureChance[ Level::Furniture::Type::Book_Case ].max = 1.0f;

    //Library Presets
    RoomPreset::Type& library = mRoomPreset.mRoomTypes[ Room::Type::Library ];

    library.width.min = 5;
    library.width.max = 8;

    library.height = library.width;

    library.furnitureCount[ Level::Furniture::Type::None ].min = 0;
    library.furnitureCount[ Level::Furniture::Type::None ].max = 0;

    library.furnitureChance[ Level::Furniture::Type::None ].min = 0.0f;
    library.furnitureChance[ Level::Furniture::Type::None ].max = 0.0f;

    library.furnitureCount[ Level::Furniture::Type::Chair ].min = 1;
    library.furnitureCount[ Level::Furniture::Type::Chair ].max = 1;

    library.furnitureChance[ Level::Furniture::Type::Chair ].min = 1.0f;
    library.furnitureChance[ Level::Furniture::Type::Chair ].max = 1.0f;

    library.furnitureCount[ Level::Furniture::Type::Desk ].min = 1;
    library.furnitureCount[ Level::Furniture::Type::Desk ].max = 3;

    library.furnitureChance[ Level::Furniture::Type::Desk ].min = 1.0f;
    library.furnitureChance[ Level::Furniture::Type::Desk ].max = 1.0f;

    library.furnitureCount[ Level::Furniture::Type::Table ].min = 1;
    library.furnitureCount[ Level::Furniture::Type::Table ].max = 3;

    library.furnitureChance[ Level::Furniture::Type::Table ].min = 1.0f;
    library.furnitureChance[ Level::Furniture::Type::Table ].max = 1.0f;

    library.furnitureCount[ Level::Furniture::Type::Bench ].min = 0;
    library.furnitureCount[ Level::Furniture::Type::Bench ].max = 0;

    library.furnitureChance[ Level::Furniture::Type::Bench ].min = 0.0f;
    library.furnitureChance[ Level::Furniture::Type::Bench ].max = 0.0f;

    library.furnitureCount[ Level::Furniture::Type::Bed_Frame ].min = 0;
    library.furnitureCount[ Level::Furniture::Type::Bed_Frame ].max = 0;

    library.furnitureChance[ Level::Furniture::Type::Bed_Frame ].min = 0.0f;
    library.furnitureChance[ Level::Furniture::Type::Bed_Frame ].max = 0.0f;

    library.furnitureCount[ Level::Furniture::Type::Book_Case ].min = 10;
    library.furnitureCount[ Level::Furniture::Type::Book_Case ].max = 15;

    library.furnitureChance[ Level::Furniture::Type::Book_Case ].min = 1.0f;
    library.furnitureChance[ Level::Furniture::Type::Book_Case ].max = 1.0f;

    //Empty Room presets
    RoomPreset::Type& storage = mRoomPreset.mRoomTypes[ Room::Type::Storage ];

    storage.width.min = 2;
    storage.width.max = 4;

    storage.height = storage.width;

    storage.furnitureCount[ Level::Furniture::Type::None ].min = 0;
    storage.furnitureCount[ Level::Furniture::Type::None ].max = 0;

    storage.furnitureChance[ Level::Furniture::Type::None ].min = 0.0f;
    storage.furnitureChance[ Level::Furniture::Type::None ].max = 0.0f;

    storage.furnitureCount[ Level::Furniture::Type::Chair ].min = 0;
    storage.furnitureCount[ Level::Furniture::Type::Chair ].max = 0;

    storage.furnitureChance[ Level::Furniture::Type::Chair ].min = 0.0f;
    storage.furnitureChance[ Level::Furniture::Type::Chair ].max = 0.0f;

    storage.furnitureCount[ Level::Furniture::Type::Desk ].min = 0;
    storage.furnitureCount[ Level::Furniture::Type::Desk ].max = 0;

    storage.furnitureChance[ Level::Furniture::Type::Desk ].min = 0.0f;
    storage.furnitureChance[ Level::Furniture::Type::Desk ].max = 0.0f;

    storage.furnitureCount[ Level::Furniture::Type::Table ].min = 0;
    storage.furnitureCount[ Level::Furniture::Type::Table ].max = 0;

    storage.furnitureChance[ Level::Furniture::Type::Table ].min = 0.0f;
    storage.furnitureChance[ Level::Furniture::Type::Table ].max = 0.0f;

    storage.furnitureCount[ Level::Furniture::Type::Bench ].min = 0;
    storage.furnitureCount[ Level::Furniture::Type::Bench ].max = 0;

    storage.furnitureChance[ Level::Furniture::Type::Bench ].min = 0.0f;
    storage.furnitureChance[ Level::Furniture::Type::Bench ].max = 0.0f;

    storage.furnitureCount[ Level::Furniture::Type::Bed_Frame ].min = 0;
    storage.furnitureCount[ Level::Furniture::Type::Bed_Frame ].max = 0;

    storage.furnitureChance[ Level::Furniture::Type::Bed_Frame ].min = 0.0f;
    storage.furnitureChance[ Level::Furniture::Type::Bed_Frame ].max = 0.0f;

    storage.furnitureCount[ Level::Furniture::Type::Book_Case ].min = 0;
    storage.furnitureCount[ Level::Furniture::Type::Book_Case ].max = 0;

    storage.furnitureChance[ Level::Furniture::Type::Book_Case ].min = 0.0f;
    storage.furnitureChance[ Level::Furniture::Type::Book_Case ].max = 0.0f;

    //Dinning Room
    RoomPreset::Type& diningRoom = mRoomPreset.mRoomTypes[ Room::Type::DiningRoom ];

    diningRoom.width.min = 4;
    diningRoom.width.max = 6;

    diningRoom.height = diningRoom.width;

    diningRoom.furnitureCount[ Level::Furniture::Type::None ].min = 0;
    diningRoom.furnitureCount[ Level::Furniture::Type::None ].max = 0;

    diningRoom.furnitureChance[ Level::Furniture::Type::None ].min = 0.0f;
    diningRoom.furnitureChance[ Level::Furniture::Type::None ].max = 0.0f;

    diningRoom.furnitureCount[ Level::Furniture::Type::Chair ].min = 4;
    diningRoom.furnitureCount[ Level::Furniture::Type::Chair ].max = 8;

    diningRoom.furnitureChance[ Level::Furniture::Type::Chair ].min = 1.0f;
    diningRoom.furnitureChance[ Level::Furniture::Type::Chair ].max = 1.0f;

    diningRoom.furnitureCount[ Level::Furniture::Type::Desk ].min = 0;
    diningRoom.furnitureCount[ Level::Furniture::Type::Desk ].max = 0;

    diningRoom.furnitureChance[ Level::Furniture::Type::Desk ].min = 0.0f;
    diningRoom.furnitureChance[ Level::Furniture::Type::Desk ].max = 0.0f;

    diningRoom.furnitureCount[ Level::Furniture::Type::Table ].min = 4;
    diningRoom.furnitureCount[ Level::Furniture::Type::Table ].max = 8;

    diningRoom.furnitureChance[ Level::Furniture::Type::Table ].min = 1.0f;
    diningRoom.furnitureChance[ Level::Furniture::Type::Table ].max = 1.0f;

    diningRoom.furnitureCount[ Level::Furniture::Type::Bench ].min = 0;
    diningRoom.furnitureCount[ Level::Furniture::Type::Bench ].max = 0;

    diningRoom.furnitureChance[ Level::Furniture::Type::Bench ].min = 0.0f;
    diningRoom.furnitureChance[ Level::Furniture::Type::Bench ].max = 0.0f;

    diningRoom.furnitureCount[ Level::Furniture::Type::Bed_Frame ].min = 0;
    diningRoom.furnitureCount[ Level::Furniture::Type::Bed_Frame ].max = 0;

    diningRoom.furnitureChance[ Level::Furniture::Type::Bed_Frame ].min = 0.0f;
    diningRoom.furnitureChance[ Level::Furniture::Type::Bed_Frame ].max = 0.0f;

    diningRoom.furnitureCount[ Level::Furniture::Type::Book_Case ].min = 0;
    diningRoom.furnitureCount[ Level::Furniture::Type::Book_Case ].max = 0;

    diningRoom.furnitureChance[ Level::Furniture::Type::Book_Case ].min = 0.0f;
    diningRoom.furnitureChance[ Level::Furniture::Type::Book_Case ].max = 0.0f;

    //Dinning Room
    RoomPreset::Type& ballRoom = mRoomPreset.mRoomTypes[ Room::Type::Ballroom ];

    ballRoom.width.min = 10;
    ballRoom.width.max = 14;

    ballRoom.height = ballRoom.width;

    ballRoom.furnitureCount[ Level::Furniture::Type::None ].min = 0;
    ballRoom.furnitureCount[ Level::Furniture::Type::None ].max = 0;

    ballRoom.furnitureChance[ Level::Furniture::Type::None ].min = 0.0f;
    ballRoom.furnitureChance[ Level::Furniture::Type::None ].max = 0.0f;

    ballRoom.furnitureCount[ Level::Furniture::Type::Chair ].min = 10;
    ballRoom.furnitureCount[ Level::Furniture::Type::Chair ].max = 14;

    ballRoom.furnitureChance[ Level::Furniture::Type::Chair ].min = 1.0f;
    ballRoom.furnitureChance[ Level::Furniture::Type::Chair ].max = 1.0f;

    ballRoom.furnitureCount[ Level::Furniture::Type::Desk ].min = 0;
    ballRoom.furnitureCount[ Level::Furniture::Type::Desk ].max = 0;

    ballRoom.furnitureChance[ Level::Furniture::Type::Desk ].min = 0.0f;
    ballRoom.furnitureChance[ Level::Furniture::Type::Desk ].max = 0.0f;

    ballRoom.furnitureCount[ Level::Furniture::Type::Table ].min = 12;
    ballRoom.furnitureCount[ Level::Furniture::Type::Table ].max = 18;

    ballRoom.furnitureChance[ Level::Furniture::Type::Table ].min = 1.0f;
    ballRoom.furnitureChance[ Level::Furniture::Type::Table ].max = 1.0f;

    ballRoom.furnitureCount[ Level::Furniture::Type::Bench ].min = 0;
    ballRoom.furnitureCount[ Level::Furniture::Type::Bench ].max = 0;

    ballRoom.furnitureChance[ Level::Furniture::Type::Bench ].min = 0.0f;
    ballRoom.furnitureChance[ Level::Furniture::Type::Bench ].max = 0.0f;

    ballRoom.furnitureCount[ Level::Furniture::Type::Bed_Frame ].min = 0;
    ballRoom.furnitureCount[ Level::Furniture::Type::Bed_Frame ].max = 0;

    ballRoom.furnitureChance[ Level::Furniture::Type::Bed_Frame ].min = 0.0f;
    ballRoom.furnitureChance[ Level::Furniture::Type::Bed_Frame ].max = 0.0f;

    ballRoom.furnitureCount[ Level::Furniture::Type::Book_Case ].min = 0;
    ballRoom.furnitureCount[ Level::Furniture::Type::Book_Case ].max = 0;

    ballRoom.furnitureChance[ Level::Furniture::Type::Book_Case ].min = 0.0f;
    ballRoom.furnitureChance[ Level::Furniture::Type::Book_Case ].max = 0.0f;
}

WorldGenerator::~WorldGenerator()
{
    if( mRooms ){
        delete[] mRooms;
    }
}

void WorldGenerator::genLevel( Level& level, LevelPreset& preset )
{
    int roomCount = mRand.gen( preset.roomCount.min, preset.roomCount.max + 1 );
    mRoomCount = roomCount;
    mRooms = new Room[ roomCount ];

    int doorCount = roomCount * 4; //The max doors we can have, generating one per side
    mDoors = new Door[ doorCount ];
    mDoorCount = 0; //No doors have been generated yet

    //Init all doors to non-essential
    for(int i = 0; i < doorCount; i++){
        mDoors[i].essential = false;
        mDoors[i].x = -1;
        mDoors[i].y = -1;
    }

    genLevelLayout( level, preset );
    
    //The door count is no longer accurate. It only represents the number of doors placed in the dungeon

    for(int i = 0; i < mRoomCount; i++){
        genLevelRoomHeights( level, preset, mRooms[i] );
        genLevelRoomWalls( level, preset, mRooms[i] );
    }

    //Separate furniture pass to make sure it all gets generated on the floor
    for(int i = 0; i < mRoomCount; i++){
        genLevelRoomFurniture( level, mRooms[i] );
    }

    LOG_INFO << "Generated Level: " << level.getWidth() << ", " << level.getDepth() << LOG_ENDL;
    LOG_INFO << "Level has " << mRoomCount << " rooms and " << mDoorCount << " doors" << LOG_ENDL;

    delete[] mRooms;
    mRooms = NULL;

    delete[] mDoors;
    mDoors = NULL;
}

void WorldGenerator::genRoom( WallSide attachSide, int attachX, int attachY, Room& room, LevelPreset& preset ) 
{
    float roomTypeRoll = mRand.genNorm();

    if( roomTypeRoll < preset.roomChances[ Room::Type::Empty ] ){
        room.type = Room::Type::Empty;
    }else if( roomTypeRoll < preset.roomChances[ Room::Type::Labyrinth ] ){
        room.type = Room::Type::Labyrinth;
    }else if( roomTypeRoll < preset.roomChances[ Room::Type::BedRoom ] ){
        room.type = Room::Type::BedRoom;
    }else if( roomTypeRoll < preset.roomChances[ Room::Type::Study ] ){
        room.type = Room::Type::Study;
    }else if( roomTypeRoll < preset.roomChances[ Room::Type::Library ] ){
        room.type = Room::Type::Library;
    }else if( roomTypeRoll < preset.roomChances[ Room::Type::Storage ] ){
        room.type = Room::Type::Storage;
    }else if( roomTypeRoll < preset.roomChances[ Room::Type::DiningRoom ] ){
        room.type = Room::Type::DiningRoom;
    }else if( roomTypeRoll <= preset.roomChances[ Room::Type::Ballroom ] ){
        room.type = Room::Type::Ballroom;
    }

    int genWidth = mRand.gen( mRoomPreset.mRoomTypes[ room.type ].width.min, mRoomPreset.mRoomTypes[ room.type ].width.max );
    int genHeight = mRand.gen( mRoomPreset.mRoomTypes[ room.type ].height.min, mRoomPreset.mRoomTypes[ room.type ].height.max );

    switch( attachSide ){
    case Back:
        if( mRand.gen(1, 100) > 50 ){
            genWidth = -genWidth;
        }
        break;
    case Right:
        if( mRand.gen(1, 100) > 50 ){
            genHeight = -genHeight;
        }
        break;
    case Front:
        genHeight = -genHeight;
        if( mRand.gen(1, 100) > 50 ){
            genWidth = -genWidth;
        }
        break;
    case Left:
        genWidth = -genWidth;
        if( mRand.gen(1, 100) > 50 ){
            genHeight = -genHeight;
        }
        break;
    default:
        break;
    }

    if( genWidth > 0 ){
        room.left = attachX;
        room.right = attachX + genWidth;
    }else{
        room.left = attachX + genWidth;
        room.right = attachX;
    }

    if( genHeight > 0 ){
        room.top = attachY;
        room.bottom = attachY + genHeight;
    }else{
        room.top = attachY + genHeight;
        room.bottom = attachY;
    }
}

void WorldGenerator::genLevelLayout( Level& level, LevelPreset& preset  )
{
    Level space;

    //Allocate a level big enough to generate a large length of rooms without
    //having to worry about hitting an edge
    space.init( mRoomCount * ( ROOM_MAX_DIMENSION + 3 ) * 2,
                mRoomCount * ( ROOM_MAX_DIMENSION + 3 ) * 2,
                mRand.gen( preset.roomCeilingHeight.min, preset.roomCeilingHeight.max + 1 ) );

    //Initialize walls EVERYWHERE
    for(int i = 0; i < space.getWidth(); i++){
        for(int j = 0; j < space.getDepth(); j++){
            space.setBlock(i, j, space.getHeight(), Level::Ramp::None );
        }
    }

    //Find the center to start so we can generate in any direction
    WallSide wallSide = (WallSide)mRand.gen(0, WallSide::Right + 1 );
    int attachedX = space.getWidth() / 2;
    int attachedY = space.getDepth() / 2;

    //Generate the rooms
    for(int i = 0; i < mRoomCount; i++){
        genRoom( wallSide, attachedX, attachedY, mRooms[ i ], preset );

        for(int x = mRooms[ i ].left; x <= mRooms[ i ].right; x++){
            for(int y = mRooms[ i ].top; y <= mRooms[ i ].bottom; y++){
                space.setBlock(x, y, 0, Level::Ramp::None);
            }
        }

        //Generate a door along each wall if there is a room on the other side
        //The door along wallSide is always unScrubable
        //We only generate doors after the first room has been genned
        if( i > 0 ){
            genDoors( space, mRooms[ i - 1 ], mRooms[ i ], wallSide );
        }

        wallSide = (WallSide)mRand.gen(0, WallSide::Right + 1 );

        switch( wallSide ){
        case Front:
            attachedY = mRooms[ i ].top - 2;
            attachedX = mRand.gen( mRooms[ i ].left, mRooms[ i ].right + 1 );
            break;
        case Left:
            attachedX = mRooms[ i ].left - 2;
            attachedY = mRand.gen( mRooms[ i ].top, mRooms[ i ].bottom + 1 );
            break;
        case Back:
            attachedY = mRooms[ i ].bottom + 2;
            attachedX = mRand.gen( mRooms[ i ].left, mRooms[ i ].right + 1 );
            break;
        case Right:
            attachedX = mRooms[ i ].right + 2;
            attachedY = mRand.gen( mRooms[ i ].top, mRooms[ i ].bottom + 1 );
            break;
        default:
            break;
        }
    }

    scrubLevelDoorways( space, preset );
    mDoorCount = applyLevelDoorways( space );

    int minX = space.getWidth();
    int maxX = 0;
    int minY = space.getDepth();
    int maxY = 0;

    //Cut out the bounds of the rooms to save space
    for( int i = 0; i < space.getWidth(); i++){
        for(int j = 0; j < space.getDepth(); j++){
            if( space.getBlockHeight(i, j) == 0 ){
                if( i > maxX ){maxX = i;}
                if( i < minX ){minX = i;}
                if( j > maxY ){maxY = j;}
                if( j < minY ){minY = j;}
            }
        }
    }

    minX-=1;
    minY-=1;

    maxX+=2;
    maxY+=2;

    //Double the scale of the generated space
    level.init( (maxX - minX) * 2, (maxY - minY) * 2, space.getHeight() );

    for( int i = 0; i < level.getWidth(); i++){
        for(int j = 0; j < level.getDepth(); j++){
            level.setBlock(i, j, space.getBlockHeight((i/2) + minX, (j/2) + minY), Level::Ramp::None );
        }
    }

    //Convert the rooms from the space level to this level
    for(int i = 0; i < mRoomCount; i++){
        mRooms[i].left -= minX;mRooms[i].left *= 2;
        mRooms[i].right -= minX; mRooms[i].right *= 2; mRooms[i].right++;
        mRooms[i].top -= minY; mRooms[i].top *= 2;
        mRooms[i].bottom -= minY; mRooms[i].bottom *= 2; mRooms[i].bottom++;
    }

    space.clear();
}

void WorldGenerator::genDoors( Level& level, Room& room, Room& prevRoom, WallSide attached )
{
    int min = 0;
    int max = 0;

    //Left Side

    //Calulcate ranges to gen from
    min = room.top > prevRoom.top ? room.top : prevRoom.top;
    max = room.bottom < prevRoom.bottom ? room.bottom : prevRoom.bottom;

    mDoors[ mDoorCount ].x = room.left - 1;

    if( min >= max ){
        mDoors[ mDoorCount ].y = min;
    }else{
        mDoors[ mDoorCount ].y = mRand.gen( min, max + 1);
    }

    //Ungenerate the door if there is no room on the other side
    if( level.getBlockHeight( mDoors[ mDoorCount ].x - 1, mDoors[ mDoorCount ].y ) == level.getHeight() ||
        level.getBlockHeight( mDoors[ mDoorCount ].x, mDoors[ mDoorCount ].y ) != level.getHeight() ){
        mDoors[ mDoorCount ].x = -1;
        mDoors[ mDoorCount ].y = -1;
    }

    if( attached == Left ){
        mDoors[ mDoorCount ].essential = true;
    }

    mDoorCount++;

    //Right Side
    min = room.top > prevRoom.top ? room.top : prevRoom.top;
    max = room.bottom < prevRoom.bottom ? room.bottom : prevRoom.bottom;

    mDoors[ mDoorCount ].x = room.right + 1;

    if( min >= max ){
        mDoors[ mDoorCount ].y = min;
    }else{
        mDoors[ mDoorCount ].y = mRand.gen( min, max + 1);
    }

    //Ungenerate the door if there is no room on the other side
    if( level.getBlockHeight( mDoors[ mDoorCount ].x + 1, mDoors[ mDoorCount ].y ) == level.getHeight() ||
        level.getBlockHeight( mDoors[ mDoorCount ].x, mDoors[ mDoorCount ].y ) != level.getHeight() ){
        mDoors[ mDoorCount ].x = -1;
        mDoors[ mDoorCount ].y = -1;
    }

    if( attached == Right ){
        mDoors[ mDoorCount ].essential = true;
    }

    mDoorCount++;

    //Front Side
    min = room.left > prevRoom.left ? room.left : prevRoom.left;
    max = room.right < prevRoom.right ? room.right : prevRoom.right;

    mDoors[ mDoorCount ].y = room.top - 1;

    if( min >= max ){
        mDoors[ mDoorCount ].x = min;
    }else{
        mDoors[ mDoorCount ].x = mRand.gen( min, max + 1);
    }

    //Ungenerate the door if there is no room on the other side
    if( level.getBlockHeight( mDoors[ mDoorCount ].x, mDoors[ mDoorCount ].y - 1 ) == level.getHeight() ||
        level.getBlockHeight( mDoors[ mDoorCount ].x, mDoors[ mDoorCount ].y ) != level.getHeight() ){
        mDoors[ mDoorCount ].x = -1;
        mDoors[ mDoorCount ].y = -1;
    }

    if( attached == Front ){
        mDoors[ mDoorCount ].essential = true;
    }

    mDoorCount++;

    //Back Side
    min = room.left > prevRoom.left ? room.left : prevRoom.left;
    max = room.right < prevRoom.right ? room.right : prevRoom.right;

    mDoors[ mDoorCount ].y = room.bottom + 1;

    if( min >= max ){
        mDoors[ mDoorCount ].x = min;
    }else{
        mDoors[ mDoorCount ].x = mRand.gen( min, max + 1);
    }

    //Ungenerate the door if there is no room on the other side
    if( level.getBlockHeight( mDoors[ mDoorCount ].x, mDoors[ mDoorCount ].y + 1 ) == level.getHeight() ||
        level.getBlockHeight( mDoors[ mDoorCount ].x, mDoors[ mDoorCount ].y ) != level.getHeight() ){
        mDoors[ mDoorCount ].x = -1;
        mDoors[ mDoorCount ].y = -1;
    }

    if( attached == Front ){
        mDoors[ mDoorCount ].essential = true;
    }

    mDoorCount++;
}


void WorldGenerator::genLevelRoomFurniture( Level& level, Room& room )
{
    //Gen each type of furniture
    int px = 0;
    int py = 0;

    //Chairs are generated by desks and tables automagically

    //Desk: Always generated against the wall
    genFurnitureInRoom( Level::Furniture::Type::Desk, level, room, true, px, py );

    //Table: Never generated against a wall
    genFurnitureInRoom( Level::Furniture::Type::Table, level, room, false, px, py );

    //Bench
    genFurnitureInRoom( Level::Furniture::Type::Bench, level, room, false, px, py );

    //Bed_Frame
    genFurnitureInRoom( Level::Furniture::Type::Bed_Frame, level, room, false, px, py );

    //Book_Case
    genFurnitureInRoom( Level::Furniture::Type::Book_Case, level, room, false, px, py );
}

void WorldGenerator::genFurnitureInRoom( Level::Furniture::Type type, Level& level, Room& room, bool againstWall, int& px, int& py )
{
    //Generate Values
    int furnCount = mRand.gen( mRoomPreset.mRoomTypes[ room.type ].furnitureCount[ type ].min, 
                               mRoomPreset.mRoomTypes[ room.type ].furnitureCount[ type ].max );
    float furnChance = ( mRand.genNorm() * ( mRoomPreset.mRoomTypes[ room.type ].furnitureChance[ type ].max -
                                       mRoomPreset.mRoomTypes[ room.type ].furnitureChance[ type ].min ) ) + 
                                       mRoomPreset.mRoomTypes[ room.type ].furnitureChance[ type ].min;
    float roll = mRand.genNorm();

    //Find furniture dimensions
    int fWidth = static_cast<int>( level.getFurnitureDimensions( type ).x / 0.3f ) + 2;
    int fHeight = static_cast<int>( level.getFurnitureDimensions( type ).z / 0.3f ) + 2;

    int halfFWidth = fWidth / 2;
    int halfFHeight = fHeight / 2;

    //If we succeed in rolling
    if( roll < furnChance ){

        int genAttempts = 0;

        //For each piece of furniture we need to generate
        for(int i = 0; i < furnCount; i++){

            //If we have tried to gen this furniture 10 times, give up bro, it aint go happen
            if( genAttempts > 10 ){
                genAttempts = 0;
                continue;
            }

            //generate positions
            int lx = mRand.gen( room.left + halfFWidth, (room.right + 1) - halfFWidth );
            int ly = mRand.gen( room.top + halfFHeight, (room.bottom + 1) - halfFHeight );

            //If we want to only generate against the wall
            if( againstWall ){
                int side = mRand.gen( 0, 4 );

                switch(side)
                {
                case 0:
                    lx = room.left;
                    ly = mRand.gen( room.top + halfFHeight, room.bottom - halfFWidth );
                    //rot = 0.0f;
                    break;
                case 1:
                    lx = room.right;
                    ly = mRand.gen( room.top + halfFHeight, room.bottom - halfFWidth );
                    //rot = 2.0f * 3.14159 * 0.5f;
                    break;
                case 2:
                    lx = mRand.gen( room.left + halfFWidth, room.right - halfFWidth );
                    //rot = 3.0f * 3.14159 * 0.5f;
                    ly = room.top;
                    break;
                case 3:
                    lx = mRand.gen( room.left + halfFWidth, room.right - halfFWidth );
                    //rot = 1.0f * 3.14159 * 0.5f;
                    ly = room.bottom;
                    break;
                default:
                    break;
                }
            }

            //calc left right top and bottom indices
            short left = lx - halfFWidth;
            short right = lx + halfFWidth;
            short top = ly - halfFHeight;
            short bottom = ly + halfFHeight;

            //Clamp the values
            CLAMP( left, room.left, room.right );
            CLAMP( right, room.left, room.right );
            CLAMP( top, room.top, room.bottom );
            CLAMP( bottom, room.top, room.bottom );

            //Make sure it doesn't intersect a wall
            if( level.getBlockHeight( lx, ly ) == level.getHeight() ||
                !level.isRectOfBlocksSameHeight( left, right, top, bottom, level.getBlockHeight( lx, ly ) ) ){
                i--;
                genAttempts++;
                continue;
            }

            float fx = (static_cast<float>(lx) * 0.3f) + 0.15f;
            float fz = (static_cast<float>(ly) * 0.3f) + 0.15f;

            int rot = 0;//mRand.gen(0, 4);

            float fl, fr, ft, fb;

            //If the furniture is rotated by 90 or 270 degrees, swap the dimensions used
            /*if( rot % 2 ){
                fl = fx - ( level.getFurnitureDimensions( type ).z / 2.0f );
                fr = fx + ( level.getFurnitureDimensions( type ).z / 2.0f );
                ft = fz - ( level.getFurnitureDimensions( type ).x / 2.0f );
                fb = fz + ( level.getFurnitureDimensions( type ).x / 2.0f );
            }else{*/
                fl = fx - ( level.getFurnitureDimensions( type ).x / 2.0f );
                fr = fx + ( level.getFurnitureDimensions( type ).x / 2.0f );
                ft = fz - ( level.getFurnitureDimensions( type ).z / 2.0f );
                fb = fz + ( level.getFurnitureDimensions( type ).z / 2.0f );
            //}

            bool failed = false;

            //Make sure it doesn't intersect other furniture.. this makes things slow as <Pick ur favorite curse word>
            for(ushort i = 0; i < level.getNumFurniture(); i++){
                Level::Furniture& f = level.getFurniture(i);
                float ofl, ofr, oft, ofb;

                //If the furniture is rotated 90 or 270 degrees, swap the dimensions used
                /*if( ( f.yRotation >= ( 3.14159f * 0.5f - 0.1f ) && 
                      f.yRotation <= ( 3.14159f * 0.5f + 0.1f ) ) ||
                    ( f.yRotation >= ( 3.0f * 3.14159f * 0.5f - 0.1f ) && 
                      f.yRotation <= ( 3.0f * 3.14159f * 0.5f + 0.1f ) ) ){
                    ofl = f.position.x - ( level.getFurnitureDimensions(f.type).z / 2.0f );
                    ofr = f.position.x + ( level.getFurnitureDimensions(f.type).z / 2.0f );
                    oft = f.position.z - ( level.getFurnitureDimensions(f.type).x / 2.0f );
                    ofb = f.position.z + ( level.getFurnitureDimensions(f.type).x / 2.0f );
                }else{*/
                    ofl = f.position.x - ( level.getFurnitureDimensions(f.type).x / 2.0f );
                    ofr = f.position.x + ( level.getFurnitureDimensions(f.type).x / 2.0f );
                    oft = f.position.z - ( level.getFurnitureDimensions(f.type).z / 2.0f );
                    ofb = f.position.z + ( level.getFurnitureDimensions(f.type).z / 2.0f );
                //}

                //Check center point first
                if( ofl <= fx && ofr >= fx &&
                    oft <= fz && ofb >= fz ){
                    failed = true;
                    break;
                }

                //Do a quick bounds check in a very dumb way
                //Top Left
                if( ofl <= fl && ofr >= fl &&
                    oft <= ft && ofb >= ft ){
                    failed = true;
                    break;
                }

                //Top Right
                if( ofl <= fr && ofr >= fr &&
                    oft <= ft && ofb >= ft ){
                    failed = true;
                    break;
                }

                //Bottom Left
                if( ofl <= fl && ofr >= fl &&
                    oft <= fb && ofb >= fb ){
                    failed = true;
                    break;
                }

                //Bottom Right
                if( ofl <= fr && ofr >= fr &&
                    oft <= fb && ofb >= fb ){
                    failed = true;
                    break;
                }
            }

            if( failed ){
                i--;
                genAttempts++;
                continue;
            }

            //Calculate height
            float height = static_cast<float>(level.getBlockHeight( lx, ly )) * 0.3f;

            //Add the furniture
            level.addFurniture( type, 
                XMFLOAT3( fx, height, fz ), 
                static_cast<float>(rot) * 3.14159f * 0.5f );

            if( type == Level::Furniture::Desk ||
                type == Level::Furniture::Table ){
                genChairByFurniture( type, level, room, lx, ly );
            }

            //Reset the number of attempts
            genAttempts = 0;
        }
    }
}

void WorldGenerator::genChairByFurniture( Level::Furniture::Type type, Level& level, Room& room, int px, int py )
{

    int fWidth = static_cast<int>( level.getFurnitureDimensions( type ).x / 0.3f ) + 2;
    int fHeight = static_cast<int>( level.getFurnitureDimensions( type ).z / 0.3f ) + 2;

    int halfFWidth = fWidth / 2;
    int halfFHeight = fHeight / 2;

    int chairWidth = static_cast<int>( level.getFurnitureDimensions( Level::Furniture::Chair ).x / 0.3f ) + 2;
    int chairHeight = static_cast<int>( level.getFurnitureDimensions( Level::Furniture::Chair ).z / 0.3f ) + 2;

    int halfCWidth = chairWidth / 2;
    int halfCHeight = chairHeight / 2;

    short left = px - halfFWidth;
    short right = px + halfFWidth;
    short top = py - halfFHeight;
    short bottom = py + halfFHeight;

    int genAttempts = 0;

    //Generate Values
    float furnCount = mRand.gen( mRoomPreset.mRoomTypes[ room.type ].furnitureCount[ type ].min, 
                           mRoomPreset.mRoomTypes[ room.type ].furnitureCount[ type ].max );
    float furnChance = ( mRand.genNorm() * ( mRoomPreset.mRoomTypes[ room.type ].furnitureChance[ type ].max -
                                       mRoomPreset.mRoomTypes[ room.type ].furnitureChance[ type ].min ) ) + 
                                       mRoomPreset.mRoomTypes[ room.type ].furnitureChance[ type ].min;
    float roll = mRand.genNorm();

    if( roll < furnChance ){
    
        for(int i = 0; i < furnCount; i++){

            if( genAttempts > 10 ){
                genAttempts = 0;
                continue;
            }

            int lx = 0;
            int ly = 0;

            int side = mRand.gen( 0, 4 );

            switch(side)
            {
            case 0:
                lx = left;
                ly = mRand.gen( top + 1, bottom );
                //rot = 0.0f;
                break;
            case 1:
                lx = right;
                ly = mRand.gen( top + 1, bottom );
                //rot = 2.0f * 3.14159 * 0.5f;
                break;
            case 2:
                lx = mRand.gen( left + 1, right );
                //rot = 3.0f * 3.14159 * 0.5f;
                ly = top;
                break;
            case 3:
                lx = mRand.gen( left + 1, right );
                //rot = 1.0f * 3.14159 * 0.5f;
                ly = bottom;
                break;
            default:
                break;
            }

            left = lx - halfFWidth;
            right = lx + halfFWidth;
            top = ly - halfFHeight;
            bottom = ly + halfFHeight;

            //Make sure we don't overstep any bounderies
            if( left < room.left || 
                right < room.left ||
                left > room.right ||
                right > room.right ||
                top < room.top || 
                bottom < room.top ||
                top > room.bottom ||
                bottom > room.bottom){
                i--;
                genAttempts++;
                continue;
            }

            //Make sure it doesn't intersect a wall
            if( level.getBlockHeight( lx, ly ) == level.getHeight() ||
                !level.isRectOfBlocksSameHeight( left, right, top, bottom, level.getBlockHeight( lx, ly ) ) ){
                i--;
                genAttempts++;
                continue;
            }

            float fx = (static_cast<float>(lx) * 0.3f) + 0.15f;
            float fz = (static_cast<float>(ly) * 0.3f) + 0.15f;

            float height = static_cast<float>(level.getBlockHeight( lx, ly )) * 0.3f;

            //Add the furniture
            level.addFurniture( Level::Furniture::Chair, 
                XMFLOAT3( fx, height, fz ), 
                static_cast<float>(0) * 3.14159f * 0.5f );

            genAttempts = 0;
        }
    }
}

void WorldGenerator::scrubLevelDoorways( Level& level, LevelPreset& preset )
{
    float scrubChance = preset.doorScrubChance.min + ( mRand.genNorm() * ( preset.doorScrubChance.max - preset.doorScrubChance.min ) ); 

    for(int i = 0; i < mDoorCount; i++){
        if( !mDoors[i].essential && mRand.genNorm() < scrubChance ){
            mDoors[i].x = -1;
            mDoors[i].y = -1;
        }
    }
}

int WorldGenerator::applyLevelDoorways( Level& level )
{
    int placed = 0;

    for(int i = 0; i < mDoorCount; i++){
        if( mDoors[i].x >= 0 && mDoors[i].y >= 0 ){
            //Average out heights around the door ignoring walls
            int totalHeight = 0;
            int validHeightCount = 0;

            if( level.getBlockHeight( mDoors[i].x - 1, mDoors[i].y ) != level.getHeight() ){
                totalHeight += level.getBlockHeight( mDoors[i].x - 1, mDoors[i].y );
                validHeightCount++;
            }

            if( level.getBlockHeight( mDoors[i].x + 1, mDoors[i].y ) != level.getHeight() ){
                totalHeight += level.getBlockHeight( mDoors[i].x + 1, mDoors[i].y );
                validHeightCount++;
            }

            if( level.getBlockHeight( mDoors[i].x, mDoors[i].y - 1) != level.getHeight() ){
                totalHeight += level.getBlockHeight( mDoors[i].x, mDoors[i].y - 1 );
                validHeightCount++;
            }

            if( level.getBlockHeight( mDoors[i].x, mDoors[i].y + 1 ) != level.getHeight() ){
                totalHeight += level.getBlockHeight( mDoors[i].x, mDoors[i].y + 1 );
                validHeightCount++;
            }

            int avg = 0;

            if( validHeightCount > 0 ){
                avg = totalHeight /= validHeightCount;
            }

            level.setBlock( mDoors[i].x, mDoors[i].y, avg, Level::Ramp::None );
            placed++;
        }
    }

    return placed;
}

void WorldGenerator::genLevelRoomWalls( Level& level, LevelPreset& preset, Room& room )
{
    float genWallDensity = mRoomPreset.wallDensity.min + ( mRand.genNorm() * ( mRoomPreset.wallDensity.max - mRoomPreset.wallDensity.min ) ); 
    int wallCount = static_cast<int>(static_cast<float>((room.right - room.left) * (room.bottom - room.top)) * genWallDensity);
    int gennedWalls = 0;
    
    while( gennedWalls < wallCount ){
        int genLength = mRand.gen( mRoomPreset.wallLength.min, mRoomPreset.wallLength.max + 1);
        int genVertical = mRand.gen(0, 100);

        int startX = mRand.gen(room.left, room.right + 1);
        int startY = mRand.gen(room.top, room.bottom + 1);
        int end = 0;

        //50/50 chance to be vertical or horizontal
        if(genVertical > 50){
            end = startY + genLength;

            CLAMP(end, room.top, room.bottom);

            for(; startY <= end; startY++){
                level.setBlock(startX, startY, level.getHeight(), Level::Ramp::None );
                gennedWalls++;
            }
        }else{
            end = startX + genLength;

            CLAMP(end, room.left, room.right);

            for(; startX <= end; startX++){
                level.setBlock(startX, startY, level.getHeight(), Level::Ramp::None );
                gennedWalls++;
            }
        }
    }
}

void WorldGenerator::genLevelRoomHeights( Level& level, LevelPreset& preset, Room& room )
{
    int maxGennedHeight = level.getHeight() - 2;
    int genHeight;

    //For these room types, don't generate height differences
    if( room.type == Room::Type::BedRoom ||
        room.type == Room::Type::Study ||
        room.type == Room::Type::Storage ||
        room.type == Room::Type::DiningRoom ||
        room.type == Room::Type::Ballroom ){
        genHeight = mRand.gen(0, maxGennedHeight + 1 );

        for(short i = room.left; i <= room.right; i++){
            for( short j = room.top; j <= room.bottom; j++){
                level.setBlock( i, j, genHeight, Level::Ramp::None );
            }
        }

        return;
    }

    //Init floor to -1
    for(short i = room.left; i <= room.right; i++){
        for( short j = room.top; j <= room.bottom; j++){
            level.setBlock( i, j, 255, Level::Ramp::None );
        }
    }

    //Generate floor rects
    while( true ){ //While we can still find empty floor to generate!
        int genArea = mRand.gen( mRoomPreset.floorSectionArea.min, mRoomPreset.floorSectionArea.max + 1 );
        genArea += (5 - (genArea % 5)); //Round up to the nearest 5
        int halfGenArea = genArea / 2;

        int* divisors = new int[ genArea ];
        int d = 0;

        //Generate divisors
        for(int i = 1; i < halfGenArea; i++){
            if( (genArea % i) == 0 ){
                divisors[d] = i;
                d++;
            }
        }

        divisors[d] = genArea;
        d++;

        //Generate which divisor to use
        int genWidth = divisors[ mRand.gen( 0, d ) ];
        int genDepth = genArea / genWidth;

        delete[] divisors;

        //Find first empty space and fill it with this the generated height
        int startI = -1;
        int startJ = 0;

        //Find the first empty block
        for(short i = room.left; i <= room.right; i++){
            for( short j = room.top; j <= room.bottom; j++){
                if( level.getBlockHeight(i, j) == 255 ){
                    startI = i;
                    startJ = j;
                    break;
                }
            }

            if( startI > -1 ){
                break;
            }
        }

        //If we didn't find any, we filled the whole thing!
        if( startI < 0 ){
            break;
        }

        //Calculate the bounds of the rect to set
        int endI = startI + genWidth;
        int endJ = startJ + genDepth;
        genHeight = mRand.gen(0, maxGennedHeight + 1 );

        //Clamp it so we don't go outside the array
        CLAMP( endI, room.left, room.right );
        CLAMP( endJ, room.left, room.bottom );

        //Loop over rect and set at genned height
        for(short i = startI ;i <= endI; i++){
            for(short j = startJ;j <= endJ; j++){
                level.setBlock( i, j, genHeight, Level::Ramp::None );
            }
        }
    }
    //Generate ramps
    float genRampDensity = mRoomPreset.rampDensity.min + ( mRand.genNorm() * ( mRoomPreset.rampDensity.max - mRoomPreset.rampDensity.min ) ); 
    int rampPct = static_cast<int>((1.0f - genRampDensity) * 100.0f);

    //Set left and right ramps
    for( short i = room.left; i <= room.right; i++){
        for(short j = room.top; j <= room.bottom; j++){

			//When generating the floor height, generate a random floor tile ID as well
            level.setBlockTileID(i,j, mRand.gen(0, mTileIDMax));

            short nextI = i;
            short nextJ = j - 1;

            if( nextJ >= room.top ){
                if( level.getBlockHeight(nextI,nextJ) != level.getHeight() && mRand.gen(1, 100) > rampPct ){
                    if( level.getBlockHeight(i,j) + 1 == level.getBlockHeight(nextI, nextJ) ){
                        level.setBlock(i, j, level.getBlockHeight(i,j), Level::Ramp::Back);
                    }
                }
            }

            nextI = i;
            nextJ = j + 1;

            if( nextJ <= room.bottom ){
                if( level.getBlockHeight(nextI,nextJ) != level.getHeight() && mRand.gen(1, 100) > rampPct ){
                    if( level.getBlockHeight(i,j) + 1 == level.getBlockHeight(nextI, nextJ) ){
                        level.setBlock(i, j, level.getBlockHeight(i,j), Level::Ramp::Front);
                    }
                }
            }
        }
    }

    //Set front and back ramps
    for( short i = room.left; i <= room.right; i++){
        for(short j = room.top; j <= room.bottom; j++){
            short nextI = i - 1;
            short nextJ = j;

            if( nextI >= room.left ){
                if( level.getBlockHeight(nextI,nextJ) != level.getHeight() ){
                    if( level.getBlockHeight(i,j) + 1 == level.getBlockHeight(nextI, nextJ) && mRand.gen(1, 100) > rampPct ){
                        level.setBlock(i, j, level.getBlockHeight(i,j), Level::Ramp::Right);
                    }
                }
            }

            nextI = i + 1;
            nextJ = j;

            if( nextI <= room.right ){
                if( level.getBlockHeight(nextI,nextJ) != level.getHeight() ){
                    if( level.getBlockHeight(i,j) + 1 == level.getBlockHeight(nextI, nextJ) && mRand.gen(1, 100) > rampPct ){
                        level.setBlock(i, j, level.getBlockHeight(i,j), Level::Ramp::Left);
                    }
                }
            }
        }
    }
}