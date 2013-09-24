#include "Level.h"

#include <assert.h>

Level::Level() :
    mWidth(0),
    mHeight(0),
    mDepth(0),
    mBlocks(0),
    mNumLights(0),
	mNumTorches(0)
{

}

Level::~Level()
{
    clear();
}

bool Level::init( short width, short depth, byte height )
{
    clear();

    mWidth = width;
    mDepth = depth;

    mHeight = height;

    mBlocks = new Block*[ mWidth ];

    for(short i = 0; i < mWidth; i++){
        mBlocks[i] = new Block[ mDepth ];

        for( short j = 0; j < mDepth; j++){
            mBlocks[i][j].height = BYTE_MAX;
            mBlocks[i][j].ramp = 0;
        }
    }

    return true;
}

bool Level::addLight( PointLight& light )
{
    if( mNumLights >= LEVEL_MAX_LIGHTS ){
        return false;
    }

    mLights[ mNumLights ] = light;
    mNumLights++;

    return true;
}

bool Level::addTorch(XMFLOAT3 pos, float rotAbootYAxis)
{
	if(mNumTorches >= LEVEL_MAX_LIGHTS)
	{
		return false;
	}

	float scale = 0.1f;
	XMMATRIX w = XMMatrixScaling(scale, scale, scale) * XMMatrixRotationAxis(XMVectorSet(0,1,0,1), rotAbootYAxis) * XMMatrixTranslation(pos.x, pos.y, pos.z);
	TorchInfo t;
	t.world = w;
	t.mesh = 0;

	mTorches[mNumTorches] = t;
	mNumTorches++;

	return true;
}

void Level::clear()
{
    if( mBlocks ){

        for(short i = 0; i < mWidth; i++){
            delete[] mBlocks[i];
        }

        delete[] mBlocks;
        mBlocks = 0;
    }

    mNumLights = 0;
	mNumTorches = 0;
}

void Level::setBlock( short i, short j, byte height, Ramp ramp )
{
    assert( i < mWidth );
    assert( j < mDepth );

    mBlocks[i][j].ramp = ramp;
    mBlocks[i][j].height = height;
}

void Level::setBlockTileID( short i, short j, byte id )
{
	assert( i < mWidth );
    assert( j < mDepth );

    mBlocks[i][j].tileId = id;
}

byte Level::getBlockTileID( short i, short j )
{
    assert( i < mWidth );
    assert( j < mDepth );

	return mBlocks[i][j].tileId;
}

Level::Ramp Level::getBlockRamp( short i, short j )
{
    assert( i < mWidth );
    assert( j < mDepth );

    return (Level::Ramp)(mBlocks[i][j].ramp);
}

byte Level::getBlockHeight( short i, short j )
{
    assert( i < mWidth );
    assert( j < mDepth );

    return mBlocks[i][j].height;
}