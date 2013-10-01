#include "Level.h"

#include <assert.h>

Level::Level() :
    mWidth(0),
    mHeight(0),
    mDepth(0),
    mBlocks(0),
    mNumLights(0),
	mNumTorches(0),
    mNumFurniture(0)
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

bool Level::addFurniture( Furniture::Type type, XMFLOAT4 position, float yRot )
{
    if( mNumFurniture >= LEVEL_MAX_LIGHTS ){
        return false;
    }

    mFurniture[ mNumFurniture ].type = type;
    mFurniture[ mNumFurniture ].position = position;
    mFurniture[ mNumFurniture ].yRotation = yRot;

    mNumFurniture++;

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
    mNumFurniture = 0;
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

bool Level::isRectOfBlocksSameHeight( short l, short r, short t, short b, byte height )
{
    assert( l < mWidth );
    assert( t < mDepth );    
    assert( r < mWidth );
    assert( b < mDepth );

    for(short j = t; j <= b; j++){
        for(short i = l; i <= r; i++){
            if( mBlocks[i][j].height != height ){
                return false;
            }
        }
    }

    return true;
}

void Level::getFurnitureAABoundingSquare( Furniture& furniture , float& left, float& front, float& right, float& back )
{
    XMVECTOR pos = XMLoadFloat4( &furniture.position );

    //Set the dimensions vectors
    XMVECTOR xDim = XMVectorSet( mFurnitureDimensions[ furniture.type ].x / 2.0f, 0.0f, 0.0f, 0.0f );
    XMVECTOR zDim = XMVectorSet( 0.0f, 0.0f, mFurnitureDimensions[ furniture.type ].z / 2.0f, 0.0f );

    //Calculate each corner from the origin
    XMVECTOR frontLeft = (-xDim - zDim);
    XMVECTOR frontRight = (xDim - zDim);
    XMVECTOR backLeft = (-xDim + zDim);
    XMVECTOR backRight = (xDim + zDim);

    //Rotate and translate
    XMMATRIX transform = XMMatrixRotationY( furniture.yRotation ) * XMMatrixTranslation( furniture.position.x, furniture.position.y, furniture.position.z );;

    //perform each transformation
    XMVECTOR transFrontLeft = XMVector4Transform( frontLeft, transform );
    XMVECTOR transFrontRight = XMVector4Transform( frontRight, transform );
    XMVECTOR transBackLeft = XMVector4Transform( backLeft, transform );
    XMVECTOR transBackRight = XMVector4Transform( backRight, transform );

    transFrontLeft += pos;
    transFrontRight += pos;
    transBackLeft += pos;
    transBackRight += pos;

    //Find the min and max boundries
    XMFLOAT4 frontLeftF4; XMStoreFloat4( &frontLeftF4, transFrontLeft );
    XMFLOAT4 frontRightF4; XMStoreFloat4( &frontRightF4, transFrontRight );
    XMFLOAT4 backLeftF4; XMStoreFloat4( &backLeftF4, transBackLeft );
    XMFLOAT4 backRightF4; XMStoreFloat4( &backRightF4, transBackRight );

    //Init, then check each float for the lowest x and lowest z as well as highest x and highest z
    left = frontLeftF4.x;
    right = frontLeftF4.x;
    front = frontLeftF4.z;
    back = frontLeftF4.z;

    //Check front right
    if( frontRightF4.x < left ){
        left = frontRightF4.x;
    }

    if( frontRightF4.x > right ){
        right = frontRightF4.x;
    }

    if( frontRightF4.z < front ){
        front = frontRightF4.z;
    }

    if( frontRightF4.z > back ){
        back = frontRightF4.z;
    }

    //check back left
    if( backLeftF4.x < left ){
        left = backLeftF4.x;
    }

    if( backLeftF4.x > right ){
        right = backLeftF4.x;
    }

    if( backLeftF4.z < front ){
        front = backLeftF4.z;
    }

    if( backLeftF4.z > back ){
        back = backLeftF4.z;
    }

    //check back right
    if( backRightF4.x < left ){
        left = backRightF4.x;
    }

    if( backRightF4.x > right ){
        right = backRightF4.x;
    }

    if( backRightF4.z < front ){
        front = backRightF4.z;
    }

    if( backRightF4.z > back ){
        back = backRightF4.z;
    }
}