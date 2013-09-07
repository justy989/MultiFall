#include "WorldGenerator.h"
#include "Utils.h"
#include "Log.h"

#include <time.h>

WorldGenerator::WorldGenerator() : 
    mRooms(NULL),
    mRoomCount(0)
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

    genLevelLayout( level, preset );
    genLevelDoorways( level, preset );

    for(int i = 0; i < mRoomCount; i++){
        genLevelRoomHeights( level, preset, mRooms[i] );
        genLevelRoomWalls( level, preset, mRooms[i] );
    }

    delete[] mRooms;
    mRooms = NULL;
}

void WorldGenerator::genRoom( WallSide attachSide, int attachX, int attachY, Room& room, LevelPreset& preset ) 
{
    int genWidth = mRand.gen( preset.roomWidth.min, preset.roomWidth.max );
    int genHeight = mRand.gen( preset.roomDepth.min, preset.roomDepth.max );

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
    space.init( mRoomCount * ( preset.roomWidth.max + 3 ) * 2,
                mRoomCount * ( preset.roomDepth.max + 3 ) * 2,
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

void WorldGenerator::genLevelDoorways( Level& level, LevelPreset& preset )
{

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
    //Init floor to -1
    for(short i = room.left; i <= room.right; i++){
        for( short j = room.top; j <= room.bottom; j++){
            level.setBlock( i, j, 255, Level::Ramp::None );
        }
    }

    //Generate floor rects
    while( true ){ //While we can still find empy floor to generate!
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

        int maxGennedHeight = level.getHeight() - 2;

        //Find first empty space and fill it with this the generated height
        int genHeight = mRand.gen(0, maxGennedHeight + 1 );
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

			//cus there's no good place for this
			level.setBlockTileID(i,j, mRand.gen(0, 16));

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