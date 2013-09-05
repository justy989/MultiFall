#include "WorldGenerator.h"
#include "Utils.h"
#include "Log.h"

#include <time.h>

WorldGenerator::WorldGenerator()
{
    mRand.seed( time( 0 ) );
}

void WorldGenerator::genRoom( Environment::Room& room )
{
    room.clear();
    room.init( mRand.gen( 8, 24 ), 
        mRand.gen( 8, 24 ),
        mRand.gen( ENV_ROOM_MAX_EXIT_HEIGHT + 1, ENV_ROOM_MAX_EXIT_HEIGHT + 6 ) );

    //Generate Exits
    int numExits = mRand.gen(2, 4);
    int sides[] = { 0, 1, 2, 3 };
    int doorCoords[4][2] = {
        {-1, -1},
        {-1, -1},
        {-1, -1},
        {-1, -1}
    } ;

    //Randomize the list of sides
    for(int i = 0; i < 4; i++){
        int gen = mRand.gen( 0, 3 );
        int tmp = sides[ gen ];
        sides[gen] = sides[i];
        sides[i] = tmp;
    }

    for(int i = 0; i < numExits; i++){

        int h = mRand.gen(0, ENV_ROOM_MAX_EXIT_HEIGHT+1);
        int l = mRand.gen(1, (sides[i] % 2) ? room.getDepth() - 2 : room.getWidth() - 2 );

        //Use the random list of sides, gen a height, then gen a location if the side is even, use the depth, else use the width
        room.setExit((Environment::Room::Exit::Side)(sides[i]), 
                      h,
                      l);

        switch( sides[i] ){
        case Environment::Room::Exit::Side::Right:
            doorCoords[sides[i]][0] = room.getWidth() - 1;
            doorCoords[sides[i]][1] = l;
            break;
        case Environment::Room::Exit::Side::Left:
            doorCoords[sides[i]][0] = 0;
            doorCoords[sides[i]][1] = l;
            break;
        case Environment::Room::Exit::Side::Front:
            doorCoords[sides[i]][0] = l;
            doorCoords[sides[i]][1] = 0;
            break;
        case Environment::Room::Exit::Side::Back:
            doorCoords[sides[i]][0] = l;
            doorCoords[sides[i]][1] = room.getDepth() - 1;
            break;
        default:
            break;
        }
    }

    int minArea = 25;
    int maxArea = 75;

    int minWidth = 5;
    int maxWidth = 10;
    int minDepth = 3;
    int maxDepth = 12;

    //Generate floor rects
    while( true ){ //While we can still find empy floor to generate!
        int genArea = mRand.gen( minArea, maxArea );
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
        int genHeight = mRand.gen(0, ENV_ROOM_MAX_EXIT_HEIGHT + 1 );
        //int genWidth = mRand.gen( minWidth, maxWidth );
        //int genDepth = mRand.gen( minDepth, maxDepth );
        int startI = -1;
        int startJ = 0;

        //Find the first empty block
        for(byte i = 0; i < room.getWidth(); i++){
            for( byte j = 0; j < room.getDepth(); j++){
                if( room.getBlockHeight(i, j) == 255 ){
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
        CLAMP( endI, 0, room.getWidth() - 1 );
        CLAMP( endJ, 0, room.getDepth() - 1 );

        //Loop over rect and set at genned height
        for(int i = startI ;i <= endI; i++){
            for(int j = startJ;j <= endJ; j++){
                room.setBlock( i, j, genHeight, Environment::Room::Block::RampDirection::None );
            }
        }
    }

    //TMP
    //Set the exit's height to be the height of the block
    for(int i = 0; i < 4; i++){
        if( doorCoords[i][0] < 0 ){
            continue;
        }

        room.setExit( (Environment::Room::Exit::Side)(i),
            room.getBlockHeight(doorCoords[i][0], doorCoords[i][1]),
            room.getExitLocation( (Environment::Room::Exit::Side)(i) ) );
    }

    //Generate ramps
    float rampMinDensity = 0.0f;
    float rampMaxDensity = 0.25f;
    float genRampDensity = rampMinDensity + ( mRand.genNorm() * ( rampMaxDensity - rampMinDensity ) ); 
    int rampPct = static_cast<int>((1.0f - genRampDensity) * 100.0f);

    //Set left and right ramps
    for( byte j = 0; j < room.getDepth(); j++){
        for(byte i = 0; i < room.getWidth(); i++){
            byte nextI = i;
            byte nextJ = j - 1;

            if( nextJ < room.getDepth() ){
                if( room.getBlockHeight(nextI,nextJ) != room.getHeight() && mRand.gen(1, 100) > rampPct ){
                    if( room.getBlockHeight(i,j) + 1 == room.getBlockHeight(nextI, nextJ) ){
                        room.setBlock(i, j, room.getBlockHeight(i,j), Environment::Room::Block::RampDirection::Back);
                        break;
                    }
                }
            }

            nextI = i;
            nextJ = j + 1;

            if( nextJ < room.getDepth() ){
                if( room.getBlockHeight(nextI,nextJ) != room.getHeight() && mRand.gen(1, 100) > rampPct ){
                    if( room.getBlockHeight(i,j) + 1 == room.getBlockHeight(nextI, nextJ) ){
                        room.setBlock(i, j, room.getBlockHeight(i,j), Environment::Room::Block::RampDirection::Front);
                        break;
                    }
                }
            }
        }
    }

    //Set front and back ramps
    for(byte i = 0; i < room.getWidth(); i++){
        for( byte j = 0; j < room.getDepth(); j++){
            byte nextI = i - 1;
            byte nextJ = j;

            if( nextI < room.getWidth() ){
                if( room.getBlockHeight(nextI,nextJ) != room.getHeight() ){
                    if( room.getBlockHeight(i,j) + 1 == room.getBlockHeight(nextI, nextJ) && mRand.gen(1, 100) > rampPct ){
                        room.setBlock(i, j, room.getBlockHeight(i,j), Environment::Room::Block::RampDirection::Right);
                        break;
                    }
                }
            }

            nextI = i + 1;
            nextJ = j;

            if( nextI < room.getWidth() ){
                if( room.getBlockHeight(nextI,nextJ) != room.getHeight() ){
                    if( room.getBlockHeight(i,j) + 1 == room.getBlockHeight(nextI, nextJ) && mRand.gen(1, 100) > rampPct ){
                        room.setBlock(i, j, room.getBlockHeight(i,j), Environment::Room::Block::RampDirection::Left);
                        break;
                    }
                }
            }
        }
    }

    //Generate Walls
    float wallMinDensity = 0.0f;
    float wallMaxDensity = 0.35f;
    int wallMinLen = 1;
    int wallMaxLen = 10;

    float genWallDensity = wallMinDensity + ( mRand.genNorm() * ( wallMaxDensity - wallMinDensity ) ); 
    int wallCount = static_cast<int>(static_cast<float>(room.getWidth() * room.getHeight()) * genWallDensity);
    int gennedWalls = 0;
    
    while( gennedWalls < wallCount ){
        int genLength = mRand.gen( wallMinLen, wallMinLen );
        int genVertical = mRand.gen(0, 100);

        int startX = mRand.gen(0, room.getWidth() - 1);
        int startY = mRand.gen(0, room.getDepth() - 1);
        int end = 0;

        //50/50 chance to be vertical or horizontal
        if(genVertical > 50){
            end = startY + genLength;

            CLAMP(end, 0, room.getDepth() - 1);

            for(; startY <= end; startY++){
                room.setBlock(startX, startY, room.getHeight(), Environment::Room::Block::RampDirection::None );
                gennedWalls++;
            }
        }else{
            end = startX + genLength;

            CLAMP(end, 0, room.getWidth() - 1);

            for(; startX <= end; startX++){
                room.setBlock(startX, startY, room.getHeight(), Environment::Room::Block::RampDirection::None );
                gennedWalls++;
            }
        }
    }
}