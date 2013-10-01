#include "WorldGenerator.h"
#include "Utils.h"
#include "Log.h"

#include <time.h>

WorldGenerator::WorldGenerator() : 
    mLevelRanges(NULL),
    mTileIDMax(16)
{
    //mRand.seed( time( 0 ) );
    mRand.seed( 41491 );

    mRoomPreset.floorSectionArea.min = 200;
    mRoomPreset.floorSectionArea.max = 300;

    mRoomPreset.rampDensity.min = 0.0f;
    mRoomPreset.rampDensity.max = 1.0f;

    mRoomPreset.wallDensity.min = 0.0f;
    mRoomPreset.wallDensity.max = 0.1f;

    mRoomPreset.wallLength.min = 1;
    mRoomPreset.wallLength.max = 16;

    //Empty presets
    RoomPreset::Type& empty = mRoomPreset.roomTypes[ Room::Type::Empty ];

    empty.dimensions.min = 2;
    empty.dimensions.max = 12;

    empty.furnitureDensity.min = 0.0f;
    empty.furnitureDensity.max = 0.0f;

    empty.furnitureChances[ Level::Furniture::Type::None ] = 0.0f;
    empty.furnitureChances[ Level::Furniture::Type::Chair ] = 0.0f;
    empty.furnitureChances[ Level::Furniture::Type::Desk ] = 0.0f;
    empty.furnitureChances[ Level::Furniture::Type::Table ] = 0.0f;
    empty.furnitureChances[ Level::Furniture::Type::Bench ] = 0.0f;
    empty.furnitureChances[ Level::Furniture::Type::Bed_Frame ] = 0.0f;
    empty.furnitureChances[ Level::Furniture::Type::Book_Case ] = 0.0f;

    //Labyrinth presets
    RoomPreset::Type& labyrinth = mRoomPreset.roomTypes[ Room::Type::Labyrinth ];

    labyrinth.dimensions.min = 2;
    labyrinth.dimensions.max = 12;

    labyrinth.furnitureDensity.min = 0.0f;
    labyrinth.furnitureDensity.max = 0.0f;

    labyrinth.furnitureChances[ Level::Furniture::Type::None ] = 0.0f;
    labyrinth.furnitureChances[ Level::Furniture::Type::Chair ] = 0.0f;
    labyrinth.furnitureChances[ Level::Furniture::Type::Desk ] = 0.0f;
    labyrinth.furnitureChances[ Level::Furniture::Type::Table ] = 0.0f;
    labyrinth.furnitureChances[ Level::Furniture::Type::Bench ] = 0.0f;
    labyrinth.furnitureChances[ Level::Furniture::Type::Bed_Frame ] = 0.0f;
    labyrinth.furnitureChances[ Level::Furniture::Type::Book_Case ] = 0.0f;

    //Bedroom presets
    RoomPreset::Type& bedRoom = mRoomPreset.roomTypes[ Room::Type::BedRoom ];

    bedRoom.dimensions.min = 2;
    bedRoom.dimensions.max = 4;

    bedRoom.furnitureDensity.min = 0.1f;
    bedRoom.furnitureDensity.max = 0.25f;

    bedRoom.furnitureChances[ Level::Furniture::Type::None ] = 0.0f;
    bedRoom.furnitureChances[ Level::Furniture::Type::Chair ] = 0.0f;
    bedRoom.furnitureChances[ Level::Furniture::Type::Desk ] = 0.1f;
    bedRoom.furnitureChances[ Level::Furniture::Type::Table ] = 0.0f;
    bedRoom.furnitureChances[ Level::Furniture::Type::Bench ] = 0.0f;
    bedRoom.furnitureChances[ Level::Furniture::Type::Bed_Frame ] = 0.8f;
    bedRoom.furnitureChances[ Level::Furniture::Type::Book_Case ] = 0.1f;

    //Study presets
    RoomPreset::Type& study = mRoomPreset.roomTypes[ Room::Type::Study ];

    study.dimensions.min = 3;
    study.dimensions.max = 5;

    study.furnitureDensity.min = 0.2f;
    study.furnitureDensity.max = 0.3f;

    study.furnitureChances[ Level::Furniture::Type::None ] = 0.0f;
    study.furnitureChances[ Level::Furniture::Type::Chair ] = 0.0f;
    study.furnitureChances[ Level::Furniture::Type::Desk ] = 0.4f;
    study.furnitureChances[ Level::Furniture::Type::Table ] = 0.1f;
    study.furnitureChances[ Level::Furniture::Type::Bench ] = 0.0f;
    study.furnitureChances[ Level::Furniture::Type::Bed_Frame ] = 0.0f;
    study.furnitureChances[ Level::Furniture::Type::Book_Case ] = 0.5f;

    //Library Presets
    RoomPreset::Type& library = mRoomPreset.roomTypes[ Room::Type::Library ];

    library.dimensions.min = 5;
    library.dimensions.max = 8;

    library.furnitureDensity.min = 0.3f;
    library.furnitureDensity.max = 0.45f;

    library.furnitureChances[ Level::Furniture::Type::None ] = 0.0f;
    library.furnitureChances[ Level::Furniture::Type::Chair ] = 0.0f;
    library.furnitureChances[ Level::Furniture::Type::Desk ] = 0.1f;
    library.furnitureChances[ Level::Furniture::Type::Table ] = 0.15f;
    library.furnitureChances[ Level::Furniture::Type::Bench ] = 0.0f;
    library.furnitureChances[ Level::Furniture::Type::Bed_Frame ] = 0.0f;
    library.furnitureChances[ Level::Furniture::Type::Book_Case ] = 0.75f;

    //Empty Room presets
    RoomPreset::Type& storage = mRoomPreset.roomTypes[ Room::Type::Storage ];

    storage.dimensions.min = 2;
    storage.dimensions.max = 4;

    storage.furnitureDensity.min = 0.0f;
    storage.furnitureDensity.max = 0.0f;

    storage.furnitureChances[ Level::Furniture::Type::None ] = 0.0f;
    storage.furnitureChances[ Level::Furniture::Type::Chair ] = 0.0f;
    storage.furnitureChances[ Level::Furniture::Type::Desk ] = 0.0f;
    storage.furnitureChances[ Level::Furniture::Type::Table ] = 0.0f;
    storage.furnitureChances[ Level::Furniture::Type::Bench ] = 0.0f;
    storage.furnitureChances[ Level::Furniture::Type::Bed_Frame ] = 0.0f;
    storage.furnitureChances[ Level::Furniture::Type::Book_Case ] = 0.0f;

    //Dinning Room
    RoomPreset::Type& diningRoom = mRoomPreset.roomTypes[ Room::Type::DiningRoom ];

    diningRoom.dimensions.min = 4;
    diningRoom.dimensions.max = 6;

    diningRoom.furnitureDensity.min = 0.0f;
    diningRoom.furnitureDensity.max = 0.0f;

    diningRoom.furnitureChances[ Level::Furniture::Type::None ] = 0.0f;
    diningRoom.furnitureChances[ Level::Furniture::Type::Chair ] = 0.0f;
    diningRoom.furnitureChances[ Level::Furniture::Type::Desk ] = 0.0f;
    diningRoom.furnitureChances[ Level::Furniture::Type::Table ] = 0.0f;
    diningRoom.furnitureChances[ Level::Furniture::Type::Bench ] = 0.0f;
    diningRoom.furnitureChances[ Level::Furniture::Type::Bed_Frame ] = 0.0f;
    diningRoom.furnitureChances[ Level::Furniture::Type::Book_Case ] = 0.0f;

    //Dinning Room
    RoomPreset::Type& ballRoom = mRoomPreset.roomTypes[ Room::Type::Ballroom ];

    ballRoom.dimensions.min = 8;
    ballRoom.dimensions.max = 12;

    ballRoom.furnitureDensity.min = 0.0f;
    ballRoom.furnitureDensity.max = 0.0f;

    ballRoom.furnitureChances[ Level::Furniture::Type::None ] = 0.0f;
    ballRoom.furnitureChances[ Level::Furniture::Type::Chair ] = 0.0f;
    ballRoom.furnitureChances[ Level::Furniture::Type::Desk ] = 0.0f;
    ballRoom.furnitureChances[ Level::Furniture::Type::Table ] = 0.0f;
    ballRoom.furnitureChances[ Level::Furniture::Type::Bench ] = 0.0f;
    ballRoom.furnitureChances[ Level::Furniture::Type::Bed_Frame ] = 0.0f;
    ballRoom.furnitureChances[ Level::Furniture::Type::Book_Case ] = 0.0f;
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
    //Generate room type
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

    //Based on type, generate width and height
    int genWidth = mRoomPreset.roomTypes[ room.type ].dimensions.genValue( mRand );
    int genHeight = mRoomPreset.roomTypes[ room.type ].dimensions.genValue( mRand );

    //Generate the direction the room is extended based on which side it is attached from
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

    //Fix bounds so left < right and top < bottom
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
                preset.roomCeilingHeight.genValue( mRand ) );

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
    //mDoorCount = applyLevelDoorways( space );

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

    room.doors[ 0 ].x = room.left - 1;

    if( min >= max ){
        room.doors[ 0 ].y = min;
    }else{
        room.doors[ 0 ].y = mRand.gen( min, max + 1 );
    }

    //Ungenerate the door if there is no room on the other side
    if( level.getBlockHeight( room.doors[ 0 ].x - 1, room.doors[ 0 ].y ) == level.getHeight() ||
        level.getBlockHeight( room.doors[ 0 ].x, room.doors[ 0 ].y ) != level.getHeight() ){
        room.doors[ 0 ].x = -1;
        room.doors[ 0 ].y = -1;
    }

    if( attached == Left ){
        room.doors[ 0 ].essential = true;
    }

    //Right Side
    min = room.top > prevRoom.top ? room.top : prevRoom.top;
    max = room.bottom < prevRoom.bottom ? room.bottom : prevRoom.bottom;

    room.doors[ 1 ].x = room.right + 1;

    if( min >= max ){
        room.doors[ 1 ].y = min;
    }else{
        room.doors[ 1 ].y = mRand.gen( min, max + 1 );
    }

    //Ungenerate the door if there is no room on the other side
    if( level.getBlockHeight( room.doors[ 1 ].x + 1, room.doors[ 1 ].y ) == level.getHeight() ||
        level.getBlockHeight( room.doors[ 1 ].x, room.doors[ 1 ].y ) != level.getHeight() ){
        room.doors[ 1 ].x = -1;
        room.doors[ 1 ].y = -1;
    }

    if( attached == Right ){
        room.doors[ 1 ].essential = true;
    }

    //Front Side
    min = room.left > prevRoom.left ? room.left : prevRoom.left;
    max = room.right < prevRoom.right ? room.right : prevRoom.right;

    room.doors[ 2 ].y = room.top - 1;

    if( min >= max ){
        room.doors[ 2 ].x = min;
    }else{
        room.doors[ 2 ].x = mRand.gen( min, max + 1);
    }

    //Ungenerate the door if there is no room on the other side
    if( level.getBlockHeight( room.doors[ 2 ].x, room.doors[ 2 ].y - 1 ) == level.getHeight() ||
        level.getBlockHeight( room.doors[ 2 ].x, room.doors[ 2 ].y ) != level.getHeight() ){
        room.doors[ 2 ].x = -1;
        room.doors[ 2 ].y = -1;
    }

    if( attached == Front ){
        room.doors[ 2 ].essential = true;
    }

    //Back Side
    min = room.left > prevRoom.left ? room.left : prevRoom.left;
    max = room.right < prevRoom.right ? room.right : prevRoom.right;

    room.doors[ 3 ].y = room.bottom + 1;

    if( min >= max ){
        room.doors[ 3 ].x = min;
    }else{
        room.doors[ 3 ].x = mRand.gen( min, max + 1);
    }

    //Ungenerate the door if there is no room on the other side
    if( level.getBlockHeight( room.doors[ 3 ].x, room.doors[ 3 ].y + 1 ) == level.getHeight() ||
        level.getBlockHeight( room.doors[ 3 ].x, room.doors[ 3 ].y ) != level.getHeight() ){
        room.doors[ 3 ].x = -1;
        room.doors[ 3 ].y = -1;
    }

    if( attached == Front ){
        room.doors[ 3 ].essential = true;
    }
}


void WorldGenerator::genLevelRoomFurniture( Level& level, Room& room )
{
    //Generate the type of furniture

    //Generate the furniture in the room

    //Is there a path to each exit?
}

void WorldGenerator::genFurnitureInRoom( Level& level, Room& room, Level::Furniture::Type type, bool againstWall, GeneratedFurniture& gennedFurniture )
{

}

void WorldGenerator::genChairByFurniture( Level::Furniture::Type type, Level& level, Room& room, int px, int py )
{
    /*
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
    float furnCount = mRand.gen( mRoomPreset.roomTypes[ room.type ].furnitureCount[ type ].min, 
                           mRoomPreset.roomTypes[ room.type ].furnitureCount[ type ].max );
    float furnChance = ( mRand.genNorm() * ( mRoomPreset.roomTypes[ room.type ].furnitureChance[ type ].max -
                                       mRoomPreset.roomTypes[ room.type ].furnitureChance[ type ].min ) ) + 
                                       mRoomPreset.roomTypes[ room.type ].furnitureChance[ type ].min;
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
    }*/
}

void WorldGenerator::scrubLevelDoorways( Level& level, LevelPreset& preset )
{
    float scrubChance = preset.doorScrubChance.genValue( mRand );

    for(int i = 0; i < mDoorCount; i++){
        if( !mDoors[i].essential && mRand.genNorm() < scrubChance ){
            mDoors[i].x = -1;
            mDoors[i].y = -1;
        }
    }
}

int WorldGenerator::applyLevelDoorways( Level& level )
{
    /*
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

    return placed;*/
    return 0;
}

void WorldGenerator::genLevelRoomWalls( Level& level, LevelPreset& preset, Room& room )
{
    float genWallDensity = mRoomPreset.wallDensity.genValue( mRand );
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
    /*
    //Skip generating ramps for now
    //Generate ramps
    float genRampDensity = mRoomPreset.rampDensity.genValue( mRand );
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
    }*/
}