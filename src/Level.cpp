#include "Level.h"

#include <assert.h>

Level::Level() :
    mWidth(0),
    mHeight(0),
    mDepth(0),
    mBlocks(0)
{

}

Level::~Level()
{
    clear();
}

bool Level::init( byte width, byte depth, byte height )
{
    clear();

    mWidth = width;
    mDepth = depth;

    mHeight = height;

    mBlocks = new Block*[ mWidth ];

    for(byte i = 0; i < mWidth; i++){
        mBlocks[i] = new Block[ mDepth ];

        for( byte j = 0; j < mDepth; j++){
            mBlocks[i][j].height = BYTE_MAX;
            mBlocks[i][j].ramp = 0;
        }
    }

    return true;
}

void Level::clear()
{
    if( mBlocks ){

        for(byte i = 0; i < mWidth; i++){
            delete[] mBlocks[i];
        }

        delete[] mBlocks;
        mBlocks = 0;
    }
}

void Level::setBlock( byte i, byte j, byte height, Block::Ramp ramp )
{
    assert( i < mWidth );
    assert( j < mDepth );

    mBlocks[i][j].ramp = ramp;
    mBlocks[i][j].height = height;
}

void Level::setBlockTileID( byte i, byte j, byte id )
{
	assert( i < mWidth );
    assert( j < mDepth );

    mBlocks[i][j].tileId = id;
}

byte Level::getBlockTileID( byte i, byte j )
{
    assert( i < mWidth );
    assert( j < mDepth );

	return mBlocks[i][j].tileId;
}

Level::Block::Ramp Level::getBlockRamp( byte i, byte j )
{
    assert( i < mWidth );
    assert( j < mDepth );

    return (Level::Block::Ramp)(mBlocks[i][j].ramp);
}

byte Level::getBlockHeight( byte i, byte j )
{
    assert( i < mWidth );
    assert( j < mDepth );

    return mBlocks[i][j].height;
}