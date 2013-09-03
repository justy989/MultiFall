#include "WorldGenerator.h"
#include "Utils.h"

#include <time.h>

WorldGenerator::WorldGenerator()
{
    mRand.seed( time( 0 ) );
}

void WorldGenerator::genRoom( Environment::Room& room )
{
    room.clear();
    room.init( mRand.gen( 8, 32), mRand.gen( 8, 32 ), mRand.gen( ENV_ROOM_MAX_EXIT_HEIGHT + 1, 8 ) );

    //Gen exits
    int numExits = mRand.gen(2, 4);
    int sides[] = { 0, 1, 2, 3 };

    //Randomize the list of sides
    for(int i = 0; i < 4; i++){
        int gen = mRand.gen( 0, 3 );
        int tmp = sides[ gen ];
        sides[gen] = sides[i];
        sides[i] = tmp;
    }

    int areaShare = room.getWidth() * room.getDepth() / numExits;
    int lowestHeight = ENV_ROOM_MAX_EXIT_HEIGHT;

    //for(int i = 0; i < numExits; i++){
    for(int i = 0; i < numExits; i++){
        //Use the random list of sides, gen a height, then gen a location if the side is even, use the depth, else use the width
        room.setExit( (Environment::Room::Exit::Side)(sides[i]), mRand.gen(0, ENV_ROOM_MAX_EXIT_HEIGHT), mRand.gen( 1, (sides[i] % 2) ? room.getDepth() : room.getWidth() ) );

        int side = i;
        int shareWidth = mRand.gen( 1, areaShare / 2 );
        int shareHeight = (areaShare / shareWidth) + 1;
        int startX, startY, endX, endY;
        
        int exitHeight = 0;

        //Based on the side, determine the starting x and y for the area of floor we want to get
        if( side == 0 ){
            exitHeight = room.getExitHeight( Environment::Room::Exit::Side::Front );
            startX = room.getExitLocation( Environment::Room::Exit::Side::Front ) - mRand.gen(0, shareWidth);
            startY = 0;
        }else if( side == 1 ){
            exitHeight = room.getExitHeight( Environment::Room::Exit::Side::Left );
            startX = 0;
            startY = room.getExitLocation( Environment::Room::Exit::Side::Left ) - mRand.gen(0, shareHeight);
        }else if( side == 2 ){
            exitHeight = room.getExitHeight( Environment::Room::Exit::Side::Back );
            startX = room.getExitLocation( Environment::Room::Exit::Side::Back ) - mRand.gen(0, shareHeight);
            startY = room.getDepth() - 1;
        }else if( side == 3 ){
            exitHeight = room.getExitHeight( Environment::Room::Exit::Side::Right );
            startX = room.getWidth() - 1;
            startY = room.getExitLocation( Environment::Room::Exit::Side::Right ) - mRand.gen(0, shareHeight);
        }

        if( exitHeight < lowestHeight ){
            lowestHeight = exitHeight;
        }

        endX = startX + shareWidth;
        endY = startY + shareHeight;

        //clamp values so they don't become invalid
        CLAMP( startX, 0, room.getWidth() - 1);
        CLAMP( startY, 0, room.getDepth() - 1);
        CLAMP( endX, 0, room.getWidth() - 1);
        CLAMP( endY, 0, room.getDepth() - 1);

        //Loop from start to end setting blocks to be the exitHeight
        for(int x = startX; x <= endX; x++){
            for(int y = startY; y <= endY; y++){
                room.setBlock( x, y, exitHeight, Environment::Room::Block::RampDirection::None );
            }
        }
    }

    //Set the rest of the room to the lowest height
    for(byte i = 0; i < room.getWidth(); i++){
        for( byte j = 0; j < room.getDepth(); j++){
            if( room.getBlockHeight( i, j ) == 0 ){
                room.setBlock(i, j, lowestHeight, Environment::Room::Block::RampDirection::None );
            }
        }
    }

    //Build some random walls
    int numWalls = mRand.gen(0, 4);

    for(int i = 0; i < numWalls; i++){
        int vertical = mRand.gen(0, 100);
        int len = mRand.gen(1, 8); //Length of the random wall

        int startX = mRand.gen(0, room.getWidth() - 1);
        int startY = mRand.gen(0, room.getDepth() - 1);
        int end = 0;

        //50/50 chance to be vertical or horizontal
        if(vertical > 50){
            end = startY + len;

            CLAMP(end, 0, room.getDepth() - 1);

            for(; startY <= end; startY++){
                room.setBlock(startX, startY, room.getHeight(), Environment::Room::Block::RampDirection::None );
            }
        }else{
            end = startX + len;

            CLAMP(end, 0, room.getWidth() - 1);

            for(; startX <= end; startX++){
                room.setBlock(startX, startY, room.getHeight(), Environment::Room::Block::RampDirection::None );
            }
        }
    }

    //Set heights randomly woooooooooo
    //for(byte i = 0; i < room.getWidth(); i++){
        //for( byte j = 0; j < room.getDepth(); j++){
            //room.setBlock( i, j, mRand.gen( 0, 3 ), Environment::Room::Block::RampDirection::None );
        //}
    //}
}