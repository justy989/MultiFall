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
    }

    for(int i = 0; i < roomCount; i++){    
        genLevelRoomFurniture( level, rooms[i] );
        //genLevelRoomContainers( level, rooms[i] );
        genLevelRoomWalls( level, rooms[i] );

        genLevelRoomLights( level, rooms[i] );
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

    //Based on type, generate width and height
    int genWidth = mLevelRanges->rooms[ room.type ].dimensions.gen( mRand );
    int genHeight = mLevelRanges->rooms[ room.type ].dimensions.gen( mRand );

    //Generate the direction the room is extended based on which side it is attached from
    switch( attachSide ){
    case Back:
        genHeight = -genHeight;
        if( mRand.gen(1, 1000) > 500 ){
            genWidth = -genWidth;
        }
        break;
    case Right:
        genWidth = -genWidth;
        if( mRand.gen(1, 1000) > 500 ){
            genHeight = -genHeight;
        }
        break;
    case Front:
        if( mRand.gen(1, 1000) > 500 ){
            genWidth = -genWidth;
        }
        break;
    case Left:
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
                mLevelRanges->roomCeilingHeight.gen( mRand ) );

    //Initialize walls EVERYWHERE
    for(int i = 0; i < space.getWidth(); i++){
        for(int j = 0; j < space.getDepth(); j++){
            space.getBlock( i, j ).setWall( space.getHeight() );
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
                space.getBlock( x, y ).setOpen();
            }
        }

        //Generate a door along each wall if there is a room on the other side
        //The door along wallSide is always unScrubable
        //We only generate doors after the first room has been genned
        if( i > 0 ){
            genDoors( space, rooms[ i ], rooms[ i - 1 ], wallSide );
        }

        wallSide = (WallSide)mRand.gen(0, WallSide::Right + 1 );

        switch( wallSide ){
        case Front:
            attachedY = rooms[ i ].bottom + 2;
            attachedX = mRand.gen( rooms[ i ].left, rooms[ i ].right + 1 );
            break;
        case Left:
            attachedX = rooms[ i ].right + 2;
            attachedY = mRand.gen( rooms[ i ].top, rooms[ i ].bottom + 1 );
            break;
        case Back:
            attachedY = rooms[ i ].top - 2;
            attachedX = mRand.gen( rooms[ i ].left, rooms[ i ].right + 1 );
            break;
        case Right:
            attachedX = rooms[ i ].left - 2;
            attachedY = mRand.gen( rooms[ i ].top, rooms[ i ].bottom + 1 );
            break;
        default:
            break;
        }
    }

    /*
    //Cutout paths for doors
    for(int i = 0; i < roomCount; i++){
        if( rooms[i].doors[Front].x >= 1 && 
            rooms[i].doors[Front].y >= 1 ){
            space.getBlock( rooms[i].doors[Front].x, rooms[i].doors[Front].y - 1 ).setHeight( 0 );
        }

        if( rooms[i].doors[Left].x >= 1 && 
            rooms[i].doors[Left].y >= 1 ){
            space.getBlock( rooms[i].doors[Left].x - 1, rooms[i].doors[Left].y ).setHeight( 0 );
        }

        if( rooms[i].doors[Back].x >= 1 && 
            rooms[i].doors[Back].y >= 1 ){
            space.getBlock( rooms[i].doors[Back].x, rooms[i].doors[Back].y + 1 ).setHeight( 0 );
        }

        if( rooms[i].doors[Right].x >= 1 && 
            rooms[i].doors[Right].y >= 1 ){
            space.getBlock( rooms[i].doors[Right].x + 1, rooms[i].doors[Right].y ).setHeight( 0 );
        }
    }*/

    //scrubLevelDoorways( space, rooms, roomCount );
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

    //Set heights based on upscaling level
    //for( int i = 0; i < level.getWidth(); i++){
        //for(int j = 0; j < level.getDepth(); j++){
            //level.getBlock(i, j).setHeight( space.getBlock( (i/2) + minX, (j/2) + minY ).getHeight() );
        //}
    //}

    for( int i = 0; i < level.getWidth(); i++){
        for(int j = 0; j < level.getDepth(); j++){
            level.getBlock( i, j ).setWall( space.getHeight() );
        }
    }

    //Convert the rooms from the space level to this level
    for(int i = 0; i < roomCount; i++){
        rooms[i].left -= minX; 
        rooms[i].left *= 2;

        rooms[i].right -= minX; 
        rooms[i].right *= 2;
        rooms[i].right++;

        rooms[i].top -= minY; 
        rooms[i].top *= 2;

        rooms[i].bottom -= minY; 
        rooms[i].bottom *= 2;
        rooms[i].bottom++;

        for(int w = rooms[i].left; w <= rooms[i].right; w++){
            for(int d = rooms[i].top; d <= rooms[i].bottom; d++){
                level.getBlock(w, d).setHeight( 0 );
                level.getBlock(w, d).setOpen();
            }
        }

        for(int d = 0; d < ROOM_MAX_DOORS; d++){
            if( rooms[i].doors[d].x >= 0 && rooms[i].doors[d].y >= 0 ){
                rooms[i].doors[d].x -= minX; 
                rooms[i].doors[d].x *= 2;

                rooms[i].doors[d].y -= minY; 
                rooms[i].doors[d].y *= 2;

                //TODO: Add a door then pass it to the setDoor call

                if( d == 2 ){
                    rooms[i].doors[d].y++;
                }

                if( d == 3 ){
                    rooms[i].doors[d].x++;
                }

                level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y ).setDoor( NULL );
                //level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y ).setHeight( 3 );

                switch(d){
                case 0:
                    level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y - 1 ).setHeight( 0 );
                    level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y - 1 ).setOpen();
                    break;
                case 1:
                    level.getBlock( rooms[i].doors[d].x - 1, rooms[i].doors[d].y ).setHeight( 0 );
                    level.getBlock( rooms[i].doors[d].x - 1, rooms[i].doors[d].y ).setOpen();
                    break;
                case 2:
                    level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y + 1 ).setHeight( 0 );
                    level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y + 1 ).setOpen();
                    break;
                case 3:
                    level.getBlock( rooms[i].doors[d].x + 1, rooms[i].doors[d].y ).setHeight( 0 );
                    level.getBlock( rooms[i].doors[d].x + 1, rooms[i].doors[d].y ).setOpen();
                    break;
                default:
                    break;
                }
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

        //Set the door in the previous room to be attached
        prevRoom.doors[ Back ].x = room.doors[ attached ].x;
        prevRoom.doors[ Back ].y = prevRoom.bottom;
        prevRoom.doors[ Back ].essential = true;

        break;

    case Left:

        //Left Side
        min = room.top > prevRoom.top ? room.top : prevRoom.top;
        max = room.bottom < prevRoom.bottom ? room.bottom : prevRoom.bottom;

        room.doors[ attached ].x = room.left;

        if( min >= max ){
            room.doors[ attached ].y = min;
        }else{
            room.doors[ attached ].y = mRand.gen( min, max + 1 );
        }

        prevRoom.doors[ Right ].x = prevRoom.right;
        prevRoom.doors[ Right ].y = room.doors[ attached ].y;
        prevRoom.doors[ Right ].essential = true;

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

        prevRoom.doors[ Front ].x = room.doors[ attached ].x;
        prevRoom.doors[ Front ].y = prevRoom.top;
        prevRoom.doors[ Front ].essential = true;

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

        prevRoom.doors[ Left ].x = prevRoom.left;
        prevRoom.doors[ Left ].y = room.doors[ attached ].y;
        prevRoom.doors[ Left ].essential = true;

        break;

    default:
        break;
    }

    room.doors[ attached ].essential = true;

    //For each wall check along it to see if doors can be created
    if( !room.doors[ Left ].essential ){
        room.doors[ Left ].x = room.left;
        room.doors[ Left ].y = mRand.gen( room.top, room.bottom + 1 );

        if( level.getBlock( room.doors[ Left ].x - 1, room.doors[ Left ].y ).getHeight() == level.getHeight() ){
            room.doors[ Left ].x = -1;
            room.doors[ Left ].y = -1;
        }
    }

    if( !room.doors[ Front ].essential ){
        room.doors[ Front ].x = mRand.gen( room.left, room.right + 1 );
        room.doors[ Front ].y = room.top;

        if( level.getBlock( room.doors[ Front ].x, room.doors[ Front ].y - 1 ).getHeight() == level.getHeight() ){
            room.doors[ Front ].x = -1;
            room.doors[ Front ].y = -1;
        }
    }

    if( !room.doors[ Right ].essential ){
        room.doors[ Right ].x = room.right;
        room.doors[ Right ].y = mRand.gen( room.top, room.bottom + 1 );

        if( level.getBlock( room.doors[ Right ].x + 1, room.doors[ Right ].y ).getHeight() == level.getHeight() ){
            room.doors[ Right ].x = -1;
            room.doors[ Right ].y = -1;
        }
    }

    if( !room.doors[ Back ].essential ){
        room.doors[ Back ].x = mRand.gen( room.left, room.right + 1 );
        room.doors[ Back ].y = room.bottom;

        if( level.getBlock( room.doors[ Back ].x, room.doors[ Back ].y + 1 ).getHeight() == level.getHeight() ){
            room.doors[ Back ].x = -1;
            room.doors[ Back ].y = -1;
        }
    }
}


void WorldGenerator::genLevelRoomFurniture( Level& level, Room& room )
{
    float genFurnitureDensity = mLevelRanges->rooms[ room.type ].furnitureDensity.gen( mRand );
    int furnitureCount = static_cast<int>(static_cast<float>((room.right - room.left) * (room.bottom - room.top)) * genFurnitureDensity);
    int gennedFurnitureBlocks = 0;

    int attempts = 0;
    
    while( gennedFurnitureBlocks < furnitureCount ){
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
        gennedFurnitureBlocks += ( ( iRight - iLeft ) + 1 ) * ( ( iBack - iFront ) + 1);
    }
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
    ushort width = (room.right - room.left) + 1;
    ushort depth = (room.bottom - room.top) + 1;

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
                int bx = room.doors[i].x - room.left;
                int by = room.doors[i].y - room.top;
                if( blocks[ bx ][ by ].getCollidableType() != 
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
        if(genDirection < 250){
            end = startY + genLength;

            CLAMP(end, room.top, room.bottom);

            for(; startY <= end; startY++){
                if( level.getBlock(startX, startY).getCollidableType() != Level::Block::Collidable::None ){
                    break;
                }

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
        }else if(genDirection < 500){
            end = startX + genLength;

            CLAMP(end, room.left, room.right);

            for(; startX <= end; startX++){
                if( level.getBlock(startX, startY).getCollidableType() != Level::Block::Collidable::None ){
                    break;
                }

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
        }else if(genDirection < 750){
            end = startY - genLength;

            CLAMP(end, room.top, room.bottom);

            for(; startY >= end; startY--){
                if( level.getBlock(startX, startY).getCollidableType() != Level::Block::Collidable::None ){
                    break;
                }

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
                if( level.getBlock(startX, startY).getCollidableType() != Level::Block::Collidable::None ){
                    break;
                }
                
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
    byte genHeight;

    //For these room types, don't generate height differences
    if( room.type == Room::Type::BedRoom ||
        room.type == Room::Type::Study ||
        room.type == Room::Type::Storage ||
        room.type == Room::Type::DiningRoom ||
        room.type == Room::Type::BallRoom ){

        genHeight = mLevelRanges->rooms[ room.type ].floorHeight.gen( mRand );

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
        genHeight = genHeight = mLevelRanges->rooms[ room.type ].floorHeight.gen( mRand );

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

void WorldGenerator::genLevelRoomLights( Level& level, Room& room )
{

}