#include "WorldGenerator.h"
#include "Utils.h"
#include "Log.h"

#include <time.h>

WorldGenerator::WorldGenerator() : 
    mLevelRanges(NULL),
    mTileIDMax(16)
{

}

WorldGenerator::~WorldGenerator()
{
    mLevelRanges = NULL;
}

void WorldGenerator::genLevel( Level& level, LevelGenerationRanges& ranges, uint seed, float blockDimension, XMFLOAT3& spawn )
{
    mRand.seed( seed );

    //Generate number of rooms that will exist in the level and allocate space for it
    int roomCount = ranges.roomCount.gen( mRand );
    Room* rooms = new Room[ roomCount ];

    mLevelRanges = &ranges;

    //Gen the rooms that make up the level
    genLevelBlueprint( level, rooms, (short)(roomCount) );

    //Set the first room to empty
    rooms[0].type = Room::Type::Empty;
    
    //For each room, do a pass generating the heights, furniture, containers, and walls
    //for(int i = 0; i < roomCount; i++){
        //genLevelRoomHeights( level, rooms[i] );
    //}

    //Do a pass on each room, generating each of the following
    for(int i = 0; i < roomCount; i++){    
        genLevelRoomWalls( level, rooms[i] );
        genLevelRoomFurniture( level, rooms[i], blockDimension );
        genLevelRoomContainers( level, rooms[i], blockDimension );
        genLevelRoomLights( level, rooms[i] );
    }

    LOG_INFO << "Generated Level: " << level.getWidth() << ", " << level.getDepth() << LOG_ENDL;
    LOG_INFO << "Level has " << roomCount << " rooms " << LOG_ENDL;

    //Set the spawn point
    for(int i = 0; i < ROOM_MAX_DOORS; i++){
        if( rooms[0].doors[i].essential ){
            spawn.x = static_cast<float>(rooms[0].doors[i].x) * blockDimension;
            spawn.y = 0.0f;
            spawn.z = static_cast<float>(rooms[0].doors[i].y) * blockDimension; 
        }
    }

    delete[] rooms;
    rooms = NULL;
}

void WorldGenerator::genRoom( WallSide attachSide, int attachX, int attachY, Room& room ) 
{
    //Generate room type
    float roomTypeRoll = mRand.genNorm();

    //Generate the type of room based on the roll
    for(int i = 0; i < Room::Type::Hallway; i++){
        if( roomTypeRoll < mLevelRanges->roomChances[ i ] ){
            room.type = (Room::Type)(i);
            break;
        }
    }

    //Hallway Roll - Override current room if we pass it
    float hallwayRoll = mRand.genNorm();

    if( hallwayRoll < mLevelRanges->roomChances[ Room::Type::Hallway ] ){
        room.type = Room::Type::Hallway;
    }

    //Based on type, generate width and height
    int genWidth = mLevelRanges->rooms[ room.type ].dimensions.gen( mRand );
    int genHeight = mLevelRanges->rooms[ room.type ].dimensions.gen( mRand );

    //Set one of the dimensions to 1 based on which side we are attached to
    if( room.type == Room::Type::Hallway ){
        switch( attachSide ){
        case WallSide::Left:
        case WallSide::Right:
            genHeight = 1;
            break;
        case WallSide::Front:
        case WallSide::Back:
            genWidth = 1;
            break;
        }
    }

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
    //Find the center to start so we can generate in any direction
    WallSide wallSide = (WallSide)mRand.gen(0, WallSide::Right + 1 );
    WallSide prevWallSide = (WallSide)((wallSide + 2) % 4);
    int attachedX = roomCount * ( ROOM_MAX_DIMENSION + 3 );
    int attachedY = roomCount * ( ROOM_MAX_DIMENSION + 3 );
    int attempts = 0;
    int minX = -1;
    int minY = -1;
    int maxX = -1;
    int maxY = -1;

    //Generate the rooms
    for(int i = 0; i < roomCount; i++){
        //If we have attempted and failed to many times, reset and regen
        if( attempts > WORLD_GEN_ATTEMPTS ){
            //RESET EVERYTHING and start overrr
            wallSide = (WallSide)mRand.gen(0, WallSide::Right + 1 );
            prevWallSide = (WallSide)((wallSide + 2) % 4);
            attachedX = roomCount * ( ROOM_MAX_DIMENSION + 3 );
            attachedY = roomCount * ( ROOM_MAX_DIMENSION + 3 );
            minX = -1;
            minY = -1;
            maxX = -1;
            maxY = -1;

            //Reset all doors
            for(int r = 0; r <= i; r++){
                for(int d = 0; d < ROOM_MAX_DOORS; d++){
                    rooms[r].doors[d].x = -1;
                    rooms[r].doors[d].y = -1;
                    rooms[r].doors[d].essential = false;
                }
            }

            i = -1;
            attempts = 0;

            continue;
        }

        genRoom( wallSide, attachedX, attachedY, rooms[ i ] );

        //If room collides with another room, try again to generate
        bool collidesWithOtherRoom = false;

        for(int p = 0; p < i; p++){
            //Top Left
            if( rooms[i].left >= rooms[p].left &&
                rooms[i].left <= rooms[p].right &&
                rooms[i].top >= rooms[p].top &&
                rooms[i].top <= rooms[p].bottom ){
                collidesWithOtherRoom = true;
                break;
            }

            //Top Right
            if( rooms[i].right >= rooms[p].left &&
                rooms[i].right <= rooms[p].right &&
                rooms[i].top >= rooms[p].top &&
                rooms[i].top <= rooms[p].bottom ){
                collidesWithOtherRoom = true;
                break;
            }

            //Bottom Left
            if( rooms[i].left >= rooms[p].left &&
                rooms[i].left <= rooms[p].right &&
                rooms[i].bottom >= rooms[p].top &&
                rooms[i].bottom <= rooms[p].bottom ){
                collidesWithOtherRoom = true;
                break;
            }

            //Bottom Right
            if( rooms[i].right >= rooms[p].left &&
                rooms[i].right <= rooms[p].right &&
                rooms[i].bottom >= rooms[p].top &&
                rooms[i].bottom <= rooms[p].bottom ){
                collidesWithOtherRoom = true;
                break;
            }
        }

        if( collidesWithOtherRoom ){
            attempts++;
            i--;
            continue;
        }

        //Generate a door along each wall if there is a room on the other side
        //The door along wallSide is always unScrubable
        //We only generate doors after the first room has been genned
        if( i > 0 ){
            genDoors( rooms, i, rooms[ i ], rooms[ i - 1 ], wallSide );
        }

        prevWallSide = (WallSide)((wallSide + 2) % 4);
        wallSide = prevWallSide;

        if( rooms[ i ].type == Room::Hallway ){
            wallSide = (WallSide)((prevWallSide + 2) % 4);
        }else{
            //Make sure the gen them differently
            while( prevWallSide == wallSide ){
                wallSide = (WallSide)mRand.gen(0, WallSide::Right + 1 );
            }
        }

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

        attempts = 0;

        if( minX < 0 ){
            minX = rooms[i].left;
        }

        if( minY < 0 ){
            minY = rooms[i].top;
        }

        if( maxX < 0 ){
            maxX = rooms[i].right;
        }

        if( maxY < 0 ){
            maxY = rooms[i].bottom;
        }

        if( minX > rooms[i].left ){
            minX = rooms[i].left;
        }

        if( maxX < rooms[i].right ){
            maxX = rooms[i].right;
        }

        if( minY > rooms[i].top ){
            minY = rooms[i].top;
        }

        if( maxY < rooms[i].bottom ){
            maxY = rooms[i].bottom;
        }
    }

    //Extend the bounds a bit
    minX-=2;
    minY-=2;

    maxX+=2;
    maxY+=2;

    //Double the scale of the generated space
    level.init( (maxX - minX) * 2, (maxY - minY) * 2, mLevelRanges->roomCeilingHeight.gen( mRand ) );

    //Initialize all to walls
    for( int i = 0; i < level.getWidth(); i++){
        for(int j = 0; j < level.getDepth(); j++){
            level.getBlock( i, j ).setWall( );
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
                //Set inside the room to be open
                level.getBlock(w, d).setOpen();

                //Randomize the tile generated
                level.getBlock(w, d).setTileID( mRand.gen(0, 16) );
            }
        }

        //For each created door
        for(int d = 0; d < ROOM_MAX_DOORS; d++){
            if( rooms[i].doors[d].x >= 0 && rooms[i].doors[d].y >= 0 ){
                rooms[i].doors[d].x -= minX;
                rooms[i].doors[d].x *= 2;

                rooms[i].doors[d].y -= minY;
                rooms[i].doors[d].y *= 2;

                if( !rooms[i].doors[d].essential ){
                    continue;
                }
                
                if( d == 2 ){
                    rooms[i].doors[d].y++;
                }

                if( d == 3 ){
                    rooms[i].doors[d].x++;
                }

                //Create and add door
                Level::Door door;
                door.getPosition().x = ( static_cast<float>( rooms[i].doors[d].x ) * 0.3f );
                door.getPosition().y = 0.0f;
                door.getPosition().z = ( static_cast<float>( rooms[i].doors[d].y ) * 0.3f );
                door.setYRotation( static_cast<float>( rooms[i].doors[d].side ) * 3.14159f * 0.5f );
                door.state = Level::Door::State::Closed;
                door.facing = (Level::Direction)( rooms[i].doors[d].side );

                //Offset position by side
                switch( rooms[i].doors[d].side ){
                case 0:
                    door.getPosition().x += 0.3f;
                    break;
                case 1:
                    break;
                case 2:
                    door.getPosition().z += 0.3f;
                    break;
                case 3:
                    door.getPosition().z += 0.3f;
                    door.getPosition().x += 0.3f;
                    break;
                default:
                    break;
                }

                ushort id = level.addDoor( door );
                Level::Door& doorRef = level.getDoor( id );

                //Set pointer to door
                level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y ).setDoor( &doorRef );

                switch(d){
                case 0:
                    level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y - 1 ).setOpen();
                    break;
                case 1:
                    level.getBlock( rooms[i].doors[d].x - 1, rooms[i].doors[d].y ).setOpen();
                    break;
                case 2:
                    level.getBlock( rooms[i].doors[d].x, rooms[i].doors[d].y + 1 ).setOpen();
                    break;
                case 3:
                    level.getBlock( rooms[i].doors[d].x + 1, rooms[i].doors[d].y ).setOpen();
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void WorldGenerator::genDoors( Room* allRooms, int currentGeneratedRoomCount,
                               Room& room, Room& prevRoom, WallSide attached )
{
    int min = 0;
    int max = 0;

    //Initialize doors to invalid
    for(int i = 0; i < ROOM_MAX_DOORS; i++){
        room.doors[i].x = -1;
        room.doors[i].y = -1;
        room.doors[i].essential = false;
    }

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

        prevRoom.doors[ Back ].x = room.doors[ attached ].x;
        prevRoom.doors[ Back ].y = prevRoom.bottom;
        prevRoom.doors[ Back ].essential = true;

        prevRoom.doors[ Back ].side = Back;

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
        
        prevRoom.doors[ Right ].side = Right;

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
        
        prevRoom.doors[ Front ].side = Front;

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
        
        prevRoom.doors[ Left ].side = Left;

        break;

    default:
        break;
    }
    
    room.doors[ attached ].side = attached;
    room.doors[ attached ].essential = true;
}


void WorldGenerator::genLevelRoomFurniture( Level& level, Room& room, float blockDimension )
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
        for(int i = Level::Furniture::Type::Desk; i < LEVEL_FURNITURE_TYPE_COUNT; i++){
            check += mLevelRanges->rooms[ room.type ].furnitureChances[ i ];

            if( furnitureRoll < check ){
                furniture.setType( (Level::Furniture::Type)(i) );
                break;
            }
        }

        //Gen position
        int gX = mRand.gen( room.left, room.right + 1 );
        int gZ = mRand.gen( room.top, room.bottom + 1 );

        //Gen rotation
        int yRot = mRand.gen( 0, 4 );

        //TODO: Regenerate values against the wall
        if( furniture.getType() == Level::Furniture::Type::Desk ){
            int wall = mRand.gen( 0, 4 );

            int furnWidth = static_cast<int>( ( level.getFurnitureDimensions( Level::Furniture::Type::Desk ).x / 2.0f ) / blockDimension );
            int furnDepth = static_cast<int>( ( level.getFurnitureDimensions( Level::Furniture::Type::Desk ).z / 2.0f ) / blockDimension );

            switch( wall ){
            case 0:
                gZ = room.top + furnDepth;
                gX = mRand.gen( room.left, room.right + 1 );
                yRot = 3;
                break;
            case 1:
                gX = room.left + furnWidth;
                gZ = mRand.gen( room.top, room.bottom + 1 );
                yRot = 0;
                break;
            case 2:
                gZ = room.bottom - furnDepth;
                gX = mRand.gen( room.left, room.right + 1 );
                yRot = 1;
                break;
            case 3:
                gX = room.right - furnWidth;
                gZ = mRand.gen( room.top, room.bottom + 1 );
                yRot = 2;
                break;
            default:
                break;
            }
        }

        //Position the furniture
        furniture.getPosition().x = ( static_cast<float>( gX ) * blockDimension ) + blockDimension / 2.0f;
        furniture.getPosition().z = ( static_cast<float>( gZ ) * blockDimension ) + blockDimension / 2.0f;
        furniture.getPosition().y = 0.0f;

        //Rotate the furniture
        furniture.setYRotation( static_cast<float>( yRot ) * ( 3.14159f / 2.0f ) );

        float left, front, right, back;
        
        //Calculate bounding box
        level.getFurnitureAABoundingSquare( furniture, left, front, right, back );

        //Convert to indices touched
        short iLeft = static_cast<short>( left / blockDimension );
        short iFront = static_cast<short>( front / blockDimension );
        short iRight = static_cast<short>( right / blockDimension );
        short iBack = static_cast<short>( back / blockDimension );

        //Make sure the area we want to set to furniture is clear
        if( !level.isRectOfBlocksOpen(iLeft, iRight, iFront, iBack) ){
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

        //Generate a chair if it is a table or desk
        if( furniture.getType() == Level::Furniture::Type::Table ||
            furniture.getType() == Level::Furniture::Type::Desk ){
            gennedFurnitureBlocks += genChairsByFurniture( level, room, furniture, iLeft, iFront, iRight, iBack );
        }
    }
}

void blockFloodFillWalkable( Level::Block** blocks, ushort width, ushort depth, ushort i, ushort j, byte fill )
{
    //Set the current block to open
    blocks[i][j].setOpen();
    blocks[i][j].setTileID( fill );

    ushort nextI = i - 1;
    ushort nextJ = j - 1;

    //Flood fill to the left
    if( nextI < width ){
        if( blocks[nextI][j].getCollidableType() <= Level::Block::Collidable::Door && 
            blocks[nextI][j].getTileID() != fill ){
            blockFloodFillWalkable( blocks, width, depth, nextI, j, fill );
        }
    }

    nextI = i + 1;

    //Flood fill to the right
    if( nextI < width ){
        if( blocks[nextI][j].getCollidableType() <= Level::Block::Collidable::Door && 
            blocks[nextI][j].getTileID() != fill ){
            blockFloodFillWalkable( blocks, width, depth, nextI, j, fill );
        }
    }

    //Flood Fill to the front
    if( nextJ < depth ){
        if( blocks[i][nextJ].getCollidableType() <= Level::Block::Collidable::Door && 
            blocks[i][nextJ].getTileID() != fill ){
            blockFloodFillWalkable( blocks, width, depth, i, nextJ, fill );
        }
    }

    nextJ = j + 1;

    //Flood fill to the back
    if( nextJ < depth ){
        if( blocks[i][nextJ].getCollidableType() <= Level::Block::Collidable::Door && 
            blocks[i][nextJ].getTileID() != fill ){
            blockFloodFillWalkable( blocks, width, depth, i, nextJ, fill );
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
            blocks[i][j].setTileID( 0 );

            switch( level.getBlock( room.left + i, room.top + j ).getCollidableType() ){
            case Level::Block::Collidable::Wall:
            case Level::Block::Collidable::Furniture:
            case Level::Block::Collidable::Container:
                blocks[i][j].setWall( );
                break;
            case Level::Block::Collidable::Door:
                blocks[i][j].setDoor( NULL );
                break;
            default:
                blocks[i][j].setOpen();
                break;
            }
        }
    }

    //Find an essential exit
    short exitI = -1, exitJ = -1;

    for(int i = 0; i < 4; i++){
        if( room.doors[i].essential ){
            exitI = room.doors[i].x - room.left;
            exitJ = room.doors[i].y - room.top;
            break;
        }
    }

    //The first room may not have an exit
    if( exitI >= 0 && exitJ >= 0 ){
        byte fill = 254;
        //Flood Fill doors, ramps and walkable space to check and see if exits are touchable
        blockFloodFillWalkable( blocks, width, depth, exitI, exitJ, fill );

        //Check each exit to make sure it still is accessible
        for(int i = 0; i < ROOM_MAX_DOORS; i++){
            if( room.doors[i].x >= 0 && room.doors[i].y >= 0 &&
                room.doors[i].essential ){
                int bx = room.doors[i].x - room.left;
                int by = room.doors[i].y - room.top;

                if( blocks[ bx ][ by ].getCollidableType() == Level::Block::Collidable::Door &&
                    blocks[ bx ][ by ].getTileID() != fill ){
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

                level.getBlock(startX, startY).setWall( );
                gennedWalls++;

                if( !pathExistsToDoors( level, room ) ){
                    level.getBlock(startX, startY).setOpen();
                    gennedWalls--;
                    attempts++;
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

                level.getBlock(startX, startY).setWall( );
                gennedWalls++;

                if( !pathExistsToDoors( level, room ) ){
                    level.getBlock(startX, startY).setOpen();
                    gennedWalls--;
                    attempts++;
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

                level.getBlock(startX, startY).setWall( );
                gennedWalls++;

                if( !pathExistsToDoors( level, room ) ){
                    level.getBlock(startX, startY).setOpen();
                    gennedWalls--;
                    attempts++;
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
                
                level.getBlock(startX, startY).setWall( );
                gennedWalls++;

                if( !pathExistsToDoors( level, room ) ){
                    level.getBlock(startX, startY).setOpen();
                    gennedWalls--;
                    attempts++;
                    break;
                }
            }
        }
    }
}


int WorldGenerator::genChairsByFurniture( Level& level, Room& room, Level::Furniture& furniture,
                                          short leftIndex, short frontIndex, short rightIndex, short backIndex )
{
    int chairCount = furniture.getType() == Level::Furniture::Desk ? mRand.gen(1, 2) : mRand.gen(1, 8 );
    int gennedChairs = 0;
    int attempts = 0;

    float chairRoll = mRand.genNorm();

    //If we fail the chair roll, don't generate any chairs
    if( chairRoll > mLevelRanges->rooms[ room.type ].furnitureChances[ Level::Furniture::Chair ] ){
        return 0;
    }

    while( gennedChairs < chairCount ){
        attempts++;

        if( attempts > WORLD_GEN_ATTEMPTS ){
            break;
        }

        int side = mRand.gen(0, 4);
        int gX = 0;
        int gZ = 0;
        int yRot = mRand.gen(0, 4);

        switch( side ){
        case 0:
            gX = leftIndex - 1;
            gZ = mRand.gen( frontIndex + 1, backIndex - 1 );
            yRot = 2;
            break;
        case 1:
            gX = mRand.gen( leftIndex + 1, rightIndex - 1 );
            gZ = frontIndex - 1;
            yRot = 1;
            break;
        case 2:
            gX = rightIndex + 1;
            gZ = mRand.gen( frontIndex + 1, backIndex - 1 );
            yRot = 0;
            break;
        case 3:
            gX = mRand.gen( leftIndex + 1, rightIndex - 1 );
            gZ = backIndex + 1;
            yRot = 3;
            break;
        default:
            break;
        }

        //If we are out of bounds of the room, try again
        if( gX < room.left || gX > room.right ||
            gZ < room.top || gZ > room.bottom ){
            continue;
        }

        Level::Furniture furniture;

        furniture.setType( Level::Furniture::Type::Chair );

        //Position the furniture
        furniture.getPosition().x = ( static_cast<float>( gX ) * 0.3f ) + 0.15f;
        furniture.getPosition().z = ( static_cast<float>( gZ ) * 0.3f ) + 0.15f;
        furniture.getPosition().y = 0.0f;

        //Rotate the furniture
        furniture.setYRotation( static_cast<float>( yRot ) * ( 3.14159f / 2.0f ) );

        float left, front, right, back;
        
        //Calculate bounding box
        level.getFurnitureAABoundingSquare( furniture, left, front, right, back );

        //Convert to indices touched
        short iLeft = static_cast<short>( left / 0.3f );
        short iFront = static_cast<short>( front / 0.3f );
        short iRight = static_cast<short>( right / 0.3f );
        short iBack = static_cast<short>( back / 0.3f );

        //Make sure the area we want to set to furniture is clear
        if( !level.isRectOfBlocksOpen(iLeft, iRight, iFront, iBack) ){
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

        if( !pathExistsToDoors( level, room ) ){
            level.removeFurniture( addedFurnitureIndex );

            for(int i = iLeft; i <= iRight; i++){
                for(int j = iFront; j <= iBack; j++){
                    level.getBlock( i, j ).setOpen();
                }
            }

            continue;
        }



        gennedChairs++;
    }

    return gennedChairs;
}

void WorldGenerator::genLevelRoomHeights( Level& level, Room& room )
{
    /*
    byte genHeight = mLevelRanges->rooms[ room.type ].floorHeight.gen( mRand );

    for(short i = room.left; i <= room.right; i++){
        for( short j = room.top; j <= room.bottom; j++){
            level.getBlock( i, j ).setWall( );
        }
    }*/
    /*
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
        CLAMP( endJ, room.top, room.bottom );

        //Loop over rect and set at genned height
        for(short i = startI ;i <= endI; i++){
            for(short j = startJ;j <= endJ; j++){
                level.getBlock( i, j ).setHeight( genHeight );
            }
        }
    }*/
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
    struct WallIndex{
        ushort x;
        ushort y;
        int side; //Front, Left, Back, Right
    };

    WallIndex* wallIndices = new WallIndex[ ( room.right - room.left ) * ( room.bottom - room.top ) * 4 ];
    ushort wallIndexCount = 0;

    for(ushort i = room.left - 1; i <= room.right + 1; i++){
        for(ushort j = room.top - 1; j <= room.bottom + 1; j++){
            if( level.getBlock( i, j ).getCollidableType() == Level::Block::Collidable::Wall ){

                if( level.getBlock( i - 1, j ).getCollidableType() != Level::Block::Collidable::Wall ){
                    wallIndices[ wallIndexCount ].x = i - 1;
                    wallIndices[ wallIndexCount ].y = j;
                    wallIndices[ wallIndexCount ].side = 3;
                    wallIndexCount++;
                }

                if( level.getBlock( i + 1, j ).getCollidableType() != Level::Block::Collidable::Wall ){
                    wallIndices[ wallIndexCount ].x = i + 1;
                    wallIndices[ wallIndexCount ].y = j;
                    wallIndices[ wallIndexCount ].side = 1;
                    wallIndexCount++;
                }

                if( level.getBlock( i, j - 1 ).getCollidableType() != Level::Block::Collidable::Wall ){
                    wallIndices[ wallIndexCount ].x = i;
                    wallIndices[ wallIndexCount ].y = j - 1;
                    wallIndices[ wallIndexCount ].side = 2;
                    wallIndexCount++;
                }

                if( level.getBlock( i, j + 1 ).getCollidableType() != Level::Block::Collidable::Wall ){
                    wallIndices[ wallIndexCount ].x = i;
                    wallIndices[ wallIndexCount ].y = j + 1;
                    wallIndices[ wallIndexCount ].side = 0;
                    wallIndexCount++;
                }
            }
        }
    }

    struct FurnitureSurfaceIndex{
        ushort x;
        ushort y;
        float h;
    };

    FurnitureSurfaceIndex* furnitureSurfaces = new FurnitureSurfaceIndex[ level.getNumFurniture() ];
    ushort furnitureSurfaceCount = 0;

    for(ushort i = 0; i < level.getNumFurniture(); i++){

        ushort fX = static_cast<int>( level.getFurniture(i).getPosition().x / 0.3f);
        ushort fY = static_cast<int>( level.getFurniture(i).getPosition().z / 0.3f);

        //If furniture is in the room we are looking at
        if( fX >= room.left && 
            fX <= room.right &&
            fY >= room.top && 
            fY <= room.bottom ){
            
            if( level.getFurniture(i).getType() == Level::Furniture::Type::Desk ||
                level.getFurniture(i).getType() == Level::Furniture::Type::Table ){
                furnitureSurfaces[ furnitureSurfaceCount ].x = fX;
                furnitureSurfaces[ furnitureSurfaceCount ].y = fY;
                furnitureSurfaces[ furnitureSurfaceCount ].h = level.getFurnitureDimensions( level.getFurniture(i).getType() ).y;
                furnitureSurfaceCount++;
            }
        }
    }

    float lightDensity = mLevelRanges->rooms[ room.type ].lightDensity.gen( mRand );
    int lightCount = static_cast<int>( static_cast<float>( wallIndexCount + furnitureSurfaceCount ) * lightDensity );
    int gennedLights = 0;
    int attempts = 0;

    while( gennedLights < lightCount ){
        if( attempts > WORLD_GEN_ATTEMPTS ){
            break;
        }

        Level::Light l;

        float check = 0.0f;
        float lightRoll = mRand.genNorm();
        Level::Light::Type genType = Level::Light::Type::None;

        //Generate the type of furniture
        for(int i = Level::Light::Type::Candle; i < LEVEL_LIGHT_TYPE_COUNT - 1; i++){ //EXCLUDE CHANDELIERS FOR NOW
            check += mLevelRanges->rooms[ room.type ].lightChances[ i ];

            if( lightRoll < check ){
                genType = (Level::Light::Type)(i);
                break;
            }
        }

        uint genIndex;

        switch( genType ){
        case Level::Light::Type::Candle:
            {
                bool collidesWithLight = false;
                genIndex = mRand.gen( 0, furnitureSurfaceCount );
                l.set( furnitureSurfaces[ genIndex ].x, furnitureSurfaces[ genIndex ].y, furnitureSurfaces[ genIndex ].h, Level::Light::Candle );

                //Make sure we don't gen lights in the same place
                for(int c = 0; c < level.getNumLights(); c++){
                    if( level.getLight(c).getI() == furnitureSurfaces[ genIndex ].x &&
                        level.getLight(c).getJ() == furnitureSurfaces[ genIndex ].y ){
                        collidesWithLight = true;
                        break;
                    }
                }

                if( !collidesWithLight ){
                    level.addLight( l );
                    gennedLights++;
                }else{
                    attempts++;
                }
            }
            break;
        case Level::Light::Type::Torch:
            {
                bool collidesWithLight = false;
                genIndex = mRand.gen( 0, wallIndexCount );
                l.set( wallIndices[ genIndex ].x, wallIndices[ genIndex ].y, 2.0f * 0.3f, Level::Light::Type::Torch, (Level::Light::AttachedWall)(wallIndices[ genIndex ].side) );
            
                //Make sure we don't gen lights in the same place
                for(int c = 0; c < level.getNumLights(); c++){
                    if( level.getLight(c).getI() == wallIndices[ genIndex ].x &&
                        level.getLight(c).getJ() == wallIndices[ genIndex ].y ){
                        collidesWithLight = true;
                        break;
                    }
                }

                if( !collidesWithLight ){
                    level.addLight( l );
                    gennedLights++;
                }else{
                    attempts++;
                }
            }
            break;
        case Level::Light::Type::Chandelier:
            attempts++;
            break;
        default:
            attempts++;
            break;
        }
    }

    delete[] wallIndices;
    delete[] furnitureSurfaces;
}

void WorldGenerator::genLevelRoomContainers( Level& level, Room& room, float blockDimension  )
{
    float genContainerDensity = mLevelRanges->rooms[ room.type ].containerDensity.gen( mRand );
    int containerCount = static_cast<int>(static_cast<float>((room.right - room.left) * (room.bottom - room.top)) * genContainerDensity);
    int gennedContainerBlocks = 0;

    int attempts = 0;
    
    while( gennedContainerBlocks < containerCount ){
        Level::Container container;

        float containerRoll = mRand.genNorm();
        float check = 0.0f;

        attempts++;

        if( attempts > WORLD_GEN_ATTEMPTS ){
            break;
        }

        //Generate the type of furniture
        for(int i = Level::Container::Type::Crate; i < Level::Container::Type::Chest; i++){
            check += mLevelRanges->rooms[ room.type ].containerChances[ i ];

            if( containerRoll < check ){
                container.setCType( (Level::Container::Type)(i) );
                break;
            }
        }

        if( container.getCType() == Level::Container::TNone ){
            attempts++;
            continue;
        }

        //Gen position
        int gX = mRand.gen( room.left, room.right + 1 );
        int gZ = mRand.gen( room.top, room.bottom + 1 );

        //Try to swindle values closer to the walls
        int swindle = mRand.gen( 0, 2 );
        int dX = room.right - gX;
        int dZ = room.bottom - gZ;

        if( swindle ){
            gX -= ( dX / 2 );
            gZ -= ( dZ / 2 );
        }else{
            gX += ( dX / 2 );
            gZ += ( dZ / 2 );
        }

        CLAMP( gX, room.left, room.right );
        CLAMP( gZ, room.top, room.bottom );

        //Gen rotation
        int yRot = mRand.gen( 0, 4 );

        //Position the furniture
        container.getPosition().x = ( static_cast<float>( gX ) * blockDimension ) + blockDimension / 2.0f;
        container.getPosition().z = ( static_cast<float>( gZ ) * blockDimension ) + blockDimension / 2.0f;
        container.getPosition().y = 0.0f;

        //Rotate the furniture
        container.setYRotation( static_cast<float>( yRot ) * ( 3.14159f / 2.0f ) );

        float left, front, right, back;
        
        //Calculate bounding box
        level.getContainerAABoundingSquare( container, left, front, right, back );

        //Convert to indices touched
        short iLeft = static_cast<short>( left / blockDimension );
        short iFront = static_cast<short>( front / blockDimension );
        short iRight = static_cast<short>( right / blockDimension );
        short iBack = static_cast<short>( back / blockDimension );

        //Make sure the area we want to set to furniture is clear
        if( !level.isRectOfBlocksOpen(iLeft, iRight, iFront, iBack) ){
            continue;
        }

        ushort addedContainerIndex = level.addContainer( container );
        Level::Container* addedContainer = &level.getContainer( addedContainerIndex );

        //Set the blocks in the touched indicies to be furniture blocks
        for(int i = iLeft; i <= iRight; i++){
            for(int j = iFront; j <= iBack; j++){
                level.getBlock( i, j ).setContainer( addedContainer );
            }
        }

        //If there isn't a path, undo what we added
        if( !pathExistsToDoors( level, room ) ){
            for(int i = iLeft; i <= iRight; i++){
                for(int j = iFront; j <= iBack; j++){
                    level.getBlock( i, j ).setOpen();
                }
            }

            level.removeContainer( addedContainerIndex );
            continue;
        }

        //Increment by furniture width * height
        gennedContainerBlocks += ( ( iRight - iLeft ) + 1 ) * ( ( iBack - iFront ) + 1);
    }
}

void WorldGenerator::genPopulation( Population& population, Level& level, PopulationGenerationRanges& ranges, float blockDimension )
{
    struct OpenBlock{
        ushort i;
        ushort j;
    };

    //Allocate space for open blocks
    ushort numOpens = 0;
    OpenBlock* opens = new OpenBlock[ level.getWidth() * level.getDepth() ];

    population.clear();

    //fill the array with open blocks
    for(ushort i = 0; i < level.getWidth(); i++){
        for(ushort j = 0; j < level.getDepth(); j++){
            if( level.getBlock(i, j).getCollidableType() == Level::Block::Collidable::None ){
                opens[ numOpens ].i = i;
                opens[ numOpens ].j = j;
                numOpens++;
            }
        }
    }

    float popDensity = ranges.density.gen( mRand );
    int popCount = static_cast<int>( static_cast<float>( numOpens ) * popDensity ) + 1;
    int gennedPop = 0;
    int attempts = 0;
    float halfBlockDimension = blockDimension * 0.5f;

    while( gennedPop < popCount ){
        if( attempts > WORLD_GEN_ATTEMPTS ){
            break;
        }

        //Generate a spot in the list
        ushort genOpen = mRand.gen( 0, numOpens );

        //Spawn an entity 0 to start
        population.spawn( 0, 
                          XMFLOAT4( static_cast<float>( opens[ genOpen ].i ) * blockDimension + halfBlockDimension,
                                    0.32f,
                                    static_cast<float>( opens[ genOpen ].j ) * blockDimension + halfBlockDimension,
                                    1.0f ) );

        //Remove the spot from the list
        numOpens--;

        if( genOpen < numOpens ){
            for(ushort i = genOpen; i < numOpens; i++){
                opens[i] = opens[i+1];
            }
        }

        gennedPop++;
    }

    delete[] opens;
}

void genItem( Item& item, ItemMaster& itemMaster, Random& random ){
    item.id = random.gen(0, itemMaster.getNumIDs() );

    ItemDefinition* def = itemMaster.getDefinitionFromID( item.id );

    if( def->stackable ){
        item.stack = random.gen(1, 20);
    }
}

void WorldGenerator::genItemsInContainers( Level& level, Population& population, ItemMaster& itemMaster )
{
    for(uint i = 0; i < level.getNumContainer(); i++){

        /*
        //20 percent chance to be emtpy
        if( mRand.gen(1, 11) > 8 ){
            continue;
        }*/

        int count = mRand.gen(1, 9);

        while( count ){
            Item item;
            genItem( item, itemMaster, mRand );
            level.getContainer(i).addItem(item);
            count--; 
        }
    }

    for(uint i = 4; i < POPULATION_MAX_CHARACTERS; i++){
        if( population.getCharacter(i).getExistence() ==
            Character::Existence::Alive ){
            //20 percent chance to be emtpy
            /*if( mRand.gen(1, 11) > 8 ){
                continue;
            }*/

            int count = mRand.gen(1, 9);

            while( count ){
                Item item;
                genItem( item, itemMaster, mRand );
                population.getCharacter(i).addItem( item );
                count--;
            }
        }
    }
}