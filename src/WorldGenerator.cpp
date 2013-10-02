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
}

WorldGenerator::~WorldGenerator()
{
    mLevelRanges = NULL;
}

void WorldGenerator::genLevel( Level& level, LevelGenerationRanges& ranges )
{
    //Generate number of rooms that will exist in the level and allocate space for it
    int roomCount = ranges.roomCount.gen( mRand );
    Room* rooms = new Room[ roomCount ];

    mLevelRanges = &ranges;

    //Gen the rooms that make up the level
    genLevelBlueprint( level, rooms, (short)(roomCount) );
    
    //For each room, do a pass generating the heights, furniture, containers, and walls
    for(int i = 0; i < roomCount; i++){
        genLevelRoomHeights( level, rooms[i] );
        genLevelRoomFurniture( level, rooms[i] );
        //genLevelRoomContainers( level, rooms[i] );
        genLevelRoomWalls( level, rooms[i] );
    }

    LOG_INFO << "Generated Level: " << level.getWidth() << ", " << level.getDepth() << LOG_ENDL;
    LOG_INFO << "Level has " << roomCount << " rooms " << LOG_ENDL;

    delete[] rooms;
    rooms = NULL;
}

void WorldGenerator::genRoom( WallSide attachSide, int attachX, int attachY, Room& room ) 
{
    //Generate room type
    float roomTypeRoll = mRand.genNorm();

    //Generate the type of room based on the roll
    for(int i = 0; i <= Room::Type::BallRoom; i++){
        if( roomTypeRoll < mLevelRanges->roomChances[ i ] ){
            room.type = (Room::Type)(i);
            break;
        }
    }

    //Hard coded to bedrooms to start
    room.type = Room::Type::BedRoom;

    //Based on type, generate width and height
    int genWidth = mLevelRanges->rooms[ room.type ].dimensions.gen( mRand );
    int genHeight = mLevelRanges->rooms[ room.type ].dimensions.gen( mRand );

    //Generate the direction the room is extended based on which side it is attached from
    switch( attachSide ){
    case Back:
        if( mRand.gen(1, 1000) > 500 ){
            genWidth = -genWidth;
        }
        break;
    case Right:
        if( mRand.gen(1, 1000) > 500 ){
            genHeight = -genHeight;
        }
        break;
    case Front:
        genHeight = -genHeight;
        if( mRand.gen(1, 1000) > 500 ){
            genWidth = -genWidth;
        }
        break;
    case Left:
        genWidth = -genWidth;
        if( mRand.gen(1, 1000) > 500 ){
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

void WorldGenerator::genLevelBlueprint( Level& level, Room* rooms, short roomCount )
{
    Level space;

    //Allocate a level big enough to generate a large length of rooms without
    //having to worry about hitting an edge
    space.init( roomCount * ( ROOM_MAX_DIMENSION + 3 ) * 2,
                roomCount * ( ROOM_MAX_DIMENSION + 3 ) * 2,
                ROOM_DEFAULT_HEIGHT );

    //Initialize walls EVERYWHERE
    for(int i = 0; i < space.getWidth(); i++){
        for(int j = 0; j < space.getDepth(); j++){
            space.getBlock( i, j ).setHeight( space.getHeight() );
        }
    }

    //Find the center to start so we can generate in any direction
    WallSide wallSide = (WallSide)mRand.gen(0, WallSide::Right + 1 );
    int attachedX = space.getWidth() / 2;
    int attachedY = space.getDepth() / 2;

    //Generate the rooms
    for(int i = 0; i < roomCount; i++){
        genRoom( wallSide, attachedX, attachedY, rooms[ i ] );

        for(int x = rooms[ i ].left; x <= rooms[ i ].right; x++){
            for(int y = rooms[ i ].top; y <= rooms[ i ].bottom; y++){
                space.getBlock( x, y ).setHeight(0);
            }
        }

        //Generate a door along each wall if there is a room on the other side
        //The door along wallSide is always unScrubable
        //We only generate doors after the first room has been genned
        if( i > 0 ){
            genDoors( space, rooms[ i - 1 ], rooms[ i ], wallSide );
        }

        wallSide = (WallSide)mRand.gen(0, WallSide::Right + 1 );

        switch( wallSide ){
        case Front:
            attachedY = rooms[ i ].top - 2;
            attachedX = mRand.gen( rooms[ i ].left, rooms[ i ].right + 1 );
            break;
        case Left:
            attachedX = rooms[ i ].left - 2;
            attachedY = mRand.gen( rooms[ i ].top, rooms[ i ].bottom + 1 );
            break;
        case Back:
            attachedY = rooms[ i ].bottom + 2;
            attachedX = mRand.gen( rooms[ i ].left, rooms[ i ].right + 1 );
            break;
        case Right:
            attachedX = rooms[ i ].right + 2;
            attachedY = mRand.gen( rooms[ i ].top, rooms[ i ].bottom + 1 );
            break;
        default:
            break;
        }
    }

    scrubLevelDoorways( space, rooms, roomCount );
    //mDoorCount = applyLevelDoorways( space );

    int minX = space.getWidth();
    int maxX = 0;
    int minY = space.getDepth();
    int maxY = 0;

    //Cut out the bounds of the rooms to save space
    for( int i = 0; i < space.getWidth(); i++){
        for(int j = 0; j < space.getDepth(); j++){
            if( space.getBlock(i, j).getHeight() == 0 ){
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
            level.getBlock(i, j).setHeight( space.getBlock( (i/2) + minX, (j/2) + minY ).getHeight() );
        }
    }

    //Convert the rooms from the space level to this level
    for(int i = 0; i < roomCount; i++){
        rooms[i].left -= minX; rooms[i].left *= 2;
        rooms[i].right -= minX; rooms[i].right *= 2; rooms[i].right++;
        rooms[i].top -= minY; rooms[i].top *= 2;
        rooms[i].bottom -= minY; rooms[i].bottom *= 2; rooms[i].bottom++;

        for(int d = 0; d < ROOM_MAX_DOORS; d++){

            if( rooms[i].doors[d].x >= 0 && rooms[i].doors[d].y >= 0 ){
                rooms[i].doors[d].x -= minX; rooms[i].doors[d].x *= 2;
                rooms[i].doors[d].y -= minY; rooms[i].doors[d].y *= 2;

                //TODO: Add a door then pass it to the setDoor call

                //level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y ).setDoor( NULL );
                //level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y ).setHeight( 0 );
            }
        }
    }

    space.clear();
}

void WorldGenerator::genDoors( Level& level, Room& room, Room& prevRoom, WallSide attached )
{
    int min = 0;
    int max = 0;

    //Generate the attached side
    switch( attached ){
    case Front:

        //Front Side
        min = room.left > prevRoom.left ? room.left : prevRoom.left;
        max = room.right < prevRoom.right ? room.right : prevRoom.right;

        room.doors[ attached ].y = room.top;

        if( min >= max ){
            room.doors[ attached ].x = min;
        }else{
            room.doors[ attached ].x = mRand.gen( min, max + 1);
        }

        break;

    case Left:

        //Left Side

        //Calulcate ranges to gen from
        min = room.top > prevRoom.top ? room.top : prevRoom.top;
        max = room.bottom < prevRoom.bottom ? room.bottom : prevRoom.bottom;

        room.doors[ attached ].x = room.left;

        if( min >= max ){
            room.doors[ attached ].y = min;
        }else{
            room.doors[ attached ].y = mRand.gen( min, max + 1 );
        }

        break;

    case Back:
        
        //Back Side
        min = room.left > prevRoom.left ? room.left : prevRoom.left;
        max = room.right < prevRoom.right ? room.right : prevRoom.right;

        room.doors[ attached ].y = room.bottom;

        if( min >= max ){
            room.doors[ attached ].x = min;
        }else{
            room.doors[ attached ].x = mRand.gen( min, max + 1);
        }

        break;

    case Right:

        //Right Side
        min = room.top > prevRoom.top ? room.top : prevRoom.top;
        max = room.bottom < prevRoom.bottom ? room.bottom : prevRoom.bottom;

        room.doors[ attached ].x = room.right;

        if( min >= max ){
            room.doors[ attached ].y = min;
        }else{
            room.doors[ attached ].y = mRand.gen( min, max + 1 );
        }

        break;

    default:
        break;
    }

    room.doors[ attached ].essential = true;

    //For each wall check along it to see if doors can be created

    /*
    //Ungenerate the door if there is no room on the other side
    if( level.getBlock( room.doors[ attached ].x + 1, room.doors[ attached ].y ).getHeight() == level.getHeight() ||
        level.getBlock( room.doors[ attached ].x, room.doors[ attached ].y ).getHeight() != level.getHeight() ){
        room.doors[ attached ].x = -1;
        room.doors[ attached ].y = -1;
    }*/
}


void WorldGenerator::genLevelRoomFurniture( Level& level, Room& room )
{
    //Generate the type of furniture

    //Generate the furniture in the room

    //Is there a path to each exit?
    
    float genFurnitureDensity = mLevelRanges->rooms[ room.type ].furnitureDensity.gen( mRand );
    int FurnitureCount = static_cast<int>(static_cast<float>((room.right - room.left) * (room.bottom - room.top)) * genFurnitureDensity);
    int gennedFurnitureBlocks = 0;

    int attempts = 0;
    
    while( gennedFurnitureBlocks < FurnitureCount ){
        Level::Furniture furniture;

        float furnitureRoll = mRand.genNorm();
        float check = 0.0f;

        attempts++;

        if( attempts > WORLD_GEN_ATTEMPTS ){
            break;
        }

        //Generate the type of furniture
        for(int i = 0; i < LEVEL_FURNITURE_TYPE_COUNT; i++){
            check += mLevelRanges->rooms[ room.type ].furnitureChances[ i ];

            if( furnitureRoll < check ){
                furniture.type = (Level::Furniture::Type)(i);
                break;
            }
        }

        //Gen position
        int gX = mRand.gen( room.left, room.right + 1 );
        int gZ = mRand.gen( room.top, room.bottom + 1 );

        //Gen rotation
        int yRot = mRand.gen( 0, 4 );

        //TODO: Regenerate values against the wall
        //if( againstWall ){
        //
        //}

        //Position the furniture
        furniture.position.x = ( static_cast<float>( gX ) * 0.3f ) + 0.15f;
        furniture.position.z = ( static_cast<float>( gZ ) * 0.3f ) + 0.15f;
        furniture.position.y = ( level.getBlock( gX, gZ ).getHeight() * 0.3f );

        //Rotate the furniture
        furniture.yRotation = static_cast<float>( yRot ) * ( 3.14159f / 2.0f );

        float left, front, right, back;
        
        //Calculate bounding box
        level.getFurnitureAABoundingSquare( furniture, left, front, right, back );

        //Convert to indices touched
        short iLeft = static_cast<short>( left / 0.3f );
        short iFront = static_cast<short>( front / 0.3f );
        short iRight = static_cast<short>( right / 0.3f );
        short iBack = static_cast<short>( back / 0.3f );

        //Make sure the area we want to set to furniture is clear
        if( !level.isRectOfBlocksLevelAndOpen(iLeft, iRight, iFront, iBack) ){
            continue;
        }

        ushort addedFurnitureIndex = level.addFurniture( furniture );
        Level::Furniture* addedFurniture = &level.getFurniture( addedFurnitureIndex );

        //Set the blocks in the touched indicies to be furniture blocks
        for(int i = iLeft; i <= iRight; i++){
            for(int j = iFront; j <= iBack; j++){
                level.getBlock( i, j ).setFurniture( addedFurniture );
            }
        }

        //If there isn't a path, undo what we added
        if( !pathExistsToDoors( level, room ) ){
            for(int i = iLeft; i <= iRight; i++){
                for(int j = iFront; j <= iBack; j++){
                    level.getBlock( i, j ).setOpen();
                }
            }

            level.removeFurniture( addedFurnitureIndex );
            continue;
        }

        //Increment by furniture width * height
        gennedFurnitureBlocks += ( iRight - iLeft ) * ( iBack - iFront );
    }
}

void WorldGenerator::genFurnitureInRoom( Level& level, Room& room, Level::Furniture::Type type, bool againstWall, GeneratedFurniture& gennedFurniture )
{

}

void blockFloodFillWalkable( Level::Block** blocks, ushort width, ushort depth, ushort i, ushort j )
{
    //Set the current block to open
    blocks[i][j].setOpen();
    blocks[i][j].setHeight( 255 );

    ushort nextI = i - 1;
    ushort nextJ = j - 1;

    //Flood fill to the left
    if( nextI < width ){
        if( blocks[nextI][j].getCollidableType() <= Level::Block::Collidable::Door && 
            blocks[nextI][j].getHeight() != 255 ){
            blockFloodFillWalkable( blocks, width, depth, nextI, j );
        }
    }

    nextI = i + 1;

    //Flood fill to the right
    if( nextI < width ){
        if( blocks[nextI][j].getCollidableType() <= Level::Block::Collidable::Door && 
            blocks[nextI][j].getHeight() != 255 ){
            blockFloodFillWalkable( blocks, width, depth, nextI, j );
        }
    }

    //Flood Fill to the front
    if( nextJ < depth ){
        if( blocks[i][nextJ].getCollidableType() <= Level::Block::Collidable::Door && 
            blocks[i][nextJ].getHeight() != 255 ){
            blockFloodFillWalkable( blocks, width, depth, i, nextJ );
        }
    }

    nextJ = j + 1;

    //Flood fill to the back
    if( nextJ < depth ){
        if( blocks[i][nextJ].getCollidableType() <= Level::Block::Collidable::Door && 
            blocks[i][nextJ].getHeight() != 255 ){
            blockFloodFillWalkable( blocks, width, depth, i, nextJ );
        }
    }
}

bool WorldGenerator::pathExistsToDoors( Level& level, Room& room )
{
    bool pathExists = true;
    ushort width = room.right - room.left;
    ushort depth = room.bottom - room.top;

    //Init block array
    Level::Block** blocks = new Level::Block*[ width ];

    for(ushort i = 0; i < width; i++){
        blocks[i] = new Level::Block[ depth ];
    }

    //Copy Blocks
    for(ushort i = 0; i < width; i++){
        for(ushort j = 0; j < depth; j++){
            blocks[i][j] = level.getBlock( room.left + i, room.top + j );
        }
    }

    //Find an essential exit
    ushort exitI = 0, exitJ = 0;

    for(int i = 0; i < 4; i++){
        if( room.doors[i].essential ){
            exitI = room.doors[i].x - room.left;
            exitJ = room.doors[i].y - room.top;
            break;
        }
    }

    //The first room may not have an exit
    if( exitI || exitJ ){
        //Flood Fill doors, ramps and walkable space to check and see if exits are touchable
        blockFloodFillWalkable( blocks, width, depth, exitI, exitJ );

        //Check each exit to make sure it still is accessible
        for(int i = 0; i < 4; i++){
            if( room.doors[i].x >= 0 && room.doors[i].y >= 0 ){
                if( blocks[ room.doors[i].x - room.left ][ room.doors[i].y - room.top ].getCollidableType() != 
                    Level::Block::Collidable::None ){
                    pathExists = false;
                    break;
                }
            }
        }
    }

    //Delete blocks
    for(ushort i = 0; i < width; i++){
        delete[] blocks[i];
    }

    delete[] blocks;

    //Return result
    return pathExists;
}

/*
void WorldGenerator::genChairByFurniture( Level::Furniture::Type type, Room& room, Level& level, GeneratedFurniture& gennedFurniture, GeneratedFurniture& gennedChair )
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
    float furnCount = mRand.gen( mRoommLevelRanges->roomTypes[ room.type ].furnitureCount[ type ].min, 
                           mRoommLevelRanges->roomTypes[ room.type ].furnitureCount[ type ].max );
    float furnChance = ( mRand.genNorm() * ( mRoommLevelRanges->roomTypes[ room.type ].furnitureChance[ type ].max -
                                       mRoommLevelRanges->roomTypes[ room.type ].furnitureChance[ type ].min ) ) + 
                                       mRoommLevelRanges->roomTypes[ room.type ].furnitureChance[ type ].min;
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
}*/

void WorldGenerator::scrubLevelDoorways( Level& level, Room* rooms, short roomCount )
{
    float scrubChance = mLevelRanges->doorScrubChance.gen( mRand );

    for(int i = 0; i < roomCount; i++){
        for(int d = 0; d < 4; d++){
            Door& door = rooms[i].doors[d];

            if( !door.essential && mRand.genNorm() < scrubChance ){
                door.x = -1;
                door.y = -1;
            }
        }
    }
}

/*
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
    return 0;
}*/

void WorldGenerator::genLevelRoomWalls( Level& level, Room& room )
{
    float genWallDensity = mLevelRanges->rooms[ room.type ].wallDensity.gen( mRand );
    int wallCount = static_cast<int>(static_cast<float>((room.right - room.left) * (room.bottom - room.top)) * genWallDensity);
    int gennedWalls = 0;
    int attempts = 0;
    
    while( gennedWalls < wallCount ){
        int genLength = mLevelRanges->rooms[ room.type ].wallLength.gen( mRand );
        int genDirection = mRand.gen(0, 1000);

        int startX = mRand.gen(room.left, room.right + 1);
        int startY = mRand.gen(room.top, room.bottom + 1);
        int end = 0;
        attempts++;

        if( attempts > WORLD_GEN_ATTEMPTS ){
            break;
        }

        //Generate an equal chance to move in any direction
        if(genDirection > 250){
            end = startY + genLength;

            CLAMP(end, room.top, room.bottom);

            for(; startY <= end; startY++){
                byte saveHeight = level.getBlock(startX, startY).getHeight();
                level.getBlock(startX, startY).setWall( level.getHeight() );
                gennedWalls++;

                if( !pathExistsToDoors( level, room ) ){
                    level.getBlock(startX, startY).setOpen();
                    level.getBlock(startX, startY).setHeight( saveHeight );
                    gennedWalls--;
                    break;
                }
            }
        }else if(genDirection > 500){
            end = startX + genLength;

            CLAMP(end, room.left, room.right);

            for(; startX <= end; startX++){
                byte saveHeight = level.getBlock(startX, startY).getHeight();
                level.getBlock(startX, startY).setWall( level.getHeight() );
                gennedWalls++;

                if( !pathExistsToDoors( level, room ) ){
                    level.getBlock(startX, startY).setOpen();
                    level.getBlock(startX, startY).setHeight( saveHeight );
                    gennedWalls--;
                    break;
                }
            }
        }else if(genDirection > 750){
            end = startY - genLength;

            CLAMP(end, room.top, room.bottom);

            for(; startY >= end; startY--){
                byte saveHeight = level.getBlock(startX, startY).getHeight();
                level.getBlock(startX, startY).setWall( level.getHeight() );
                gennedWalls++;

                if( !pathExistsToDoors( level, room ) ){
                    level.getBlock(startX, startY).setOpen();
                    level.getBlock(startX, startY).setHeight( saveHeight );
                    gennedWalls--;
                    break;
                }
            }
        }else{
            end = startX - genLength;

            CLAMP(end, room.left, room.right);

            for(; startX >= end; startX--){
                byte saveHeight = level.getBlock(startX, startY).getHeight();
                level.getBlock(startX, startY).setWall( level.getHeight() );
                gennedWalls++;

                if( !pathExistsToDoors( level, room ) ){
                    level.getBlock(startX, startY).setOpen();
                    level.getBlock(startX, startY).setHeight( saveHeight );
                    gennedWalls--;
                    break;
                }
            }
        }
    }
}

void WorldGenerator::genLevelRoomHeights( Level& level, Room& room )
{
    int maxGennedHeight = level.getHeight() - 2;
    int genHeight;

    //For these room types, don't generate height differences
    if( room.type == Room::Type::BedRoom ||
        room.type == Room::Type::Study ||
        room.type == Room::Type::Storage ||
        room.type == Room::Type::DiningRoom ||
        room.type == Room::Type::BallRoom ){

        genHeight = mRand.gen(0, maxGennedHeight + 1 );

        for(short i = room.left; i <= room.right; i++){
            for( short j = room.top; j <= room.bottom; j++){
                level.getBlock( i, j ).setHeight( genHeight );
            }
        }

        return;
    }

    //Init floor to -1
    for(short i = room.left; i <= room.right; i++){
        for( short j = room.top; j <= room.bottom; j++){
                level.getBlock( i, j ).setHeight( 255 );
        }
    }

    //Generate floor rects
    while( true ){ //While we can still find empty floor to generate!
        int genArea = mLevelRanges->rooms[ room.type ].floorSectionArea.gen( mRand );
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
                if( level.getBlock(i, j).getHeight() == 255 ){
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
                level.getBlock( i, j ).setHeight( genHeight );
            }
        }
    }
    /*
    //Skip generating ramps for now
    //Generate ramps
    float genRampDensity = mRoommLevelRanges->rampDensity.genValue( mRand );
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