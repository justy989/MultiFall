#include "Environment.h"

#include <assert.h>

Environment::Environment()
{

}

Environment::Room::Room() :
    mWidth(0),
    mHeight(0),
    mDepth(0),
    mBlocks(0)
{

}

Environment::Room::~Room()
{
    clear();
}

bool Environment::Room::init( byte width, byte depth, byte height )
{
    clear();

    mWidth = width;
    mDepth = depth;

    assert( height <= ENV_ROOM_MAX_HEIGHT );

    mHeight = height;

    mBlocks = new Block*[ mWidth ];

    for(byte i = 0; i < mWidth; i++){
        mBlocks[i] = new Block[ mDepth ];

        for( byte j = 0; j < mDepth; j++){
            mBlocks[i][j].height = 0;
            mBlocks[i][j].ramp = 0;
        }
    }

    return true;
}

void Environment::Room::clear()
{
    if( mBlocks ){

        for(byte i = 0; i < mWidth; i++){
            delete[] mBlocks[i];
        }

        delete[] mBlocks;
        mBlocks = 0;
    }

    for(byte i = 0; i < 4; i++){
        mExits[i].height = 0;
        mExits[i].location = 0;
    }
}

void Environment::Room::setExit( Exit::Side side, byte height, byte location )
{
    assert( height <= ENV_ROOM_MAX_EXIT_HEIGHT );
    assert( side <= Exit::Side::Right );

    mExits[side].height = height;
    mExits[side].location = location;
}

void Environment::Room::setBlock( byte i, byte j, byte height, Block::RampDirection ramp )
{
    assert( i < mWidth );
    assert( j < mDepth );

    mBlocks[i][j].ramp = ramp;
    mBlocks[i][j].height = height;
}

Environment::Room::Block::RampDirection Environment::Room::getBlockRamp( byte i, byte j )
{
    assert( i < mWidth );
    assert( j < mDepth );

    return (Environment::Room::Block::RampDirection)(mBlocks[i][j].ramp);
}

byte Environment::Room::getBlockHeight( byte i, byte j )
{
    assert( i < mWidth );
    assert( j < mDepth );

    return mBlocks[i][j].height;
}

byte Environment::Room::getExitHeight( Exit::Side  side )
{
    assert( side <= Exit::Side::Right );

    return mExits[side].height;
}

byte Environment::Room::getExitLocation( Exit::Side side )
{
    assert( side <= Exit::Side::Right );

    return mExits[side].location;
}