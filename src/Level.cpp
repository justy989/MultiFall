#include "Level.h"

#include <assert.h>

Level::Block::Block() : 
    mCollidable(Collidable::None),
    mTileID(0),
    mPadding(0),
    mDoor(NULL)
{

}

Level::Level() :
    mWidth(0),
    mHeight(0),
    mDepth(0),
    mBlocks(0),
    mNumLights(0),
    mNumFurniture(0),
    mNumContainers(0),
    mNumDoors(0)
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
    }

    return true;
}

bool Level::addLight( Light& light )
{
    if( mNumLights >= LEVEL_MAX_LIGHTS ){
        return false;
    }

    mLights[ mNumLights ] = light;
    mNumLights++;

    return true;
}

ushort Level::addFurniture( Furniture::Type type, XMFLOAT4 position, float yRot )
{
    if( mNumFurniture >= LEVEL_MAX_FURNITURE ){
        return false;
    }

    mFurniture[ mNumFurniture ].setType( type );
    mFurniture[ mNumFurniture ].getPosition() = position ;
    mFurniture[ mNumFurniture ].setYRotation( yRot );

    mNumFurniture++;

    return mNumFurniture - 1;
}

ushort Level::addFurniture( Furniture& furniture )
{
    if( mNumFurniture >= LEVEL_MAX_FURNITURE ){
        return false;
    }

    mFurniture[ mNumFurniture ] = furniture;

    mNumFurniture++;

    return mNumFurniture - 1;
}

bool Level::removeFurniture( ushort index )
{
    if( index >= mNumFurniture ){
        return false;
    }

    mNumFurniture--;

    for(ushort i = index; i < mNumFurniture; i++){
        mFurniture[i] = mFurniture[i+1];
    }

    return true;
}

ushort Level::addContainer( Level::Container& container )
{
    if( mNumContainers >= LEVEL_MAX_CONTAINERS ){
        return false;
    }

    mContainers[ mNumContainers ] = container;

    mNumContainers++;
    
    return mNumContainers - 1;
}

bool Level::removeContainer( ushort index )
{
    if( index >= mNumContainers ){
        return false;
    }

    mNumContainers--;

    for(ushort i = index; i < mNumContainers; i++){
        mContainers[i] = mContainers[i+1];
    }

    return true;
}

ushort Level::addDoor( Door& door )
{
    if( mNumDoors >= LEVEL_MAX_DOORS ){
        return false;
    }

    mDoors[ mNumDoors ] = door;

    mNumDoors++;
    
    return mNumDoors - 1;
}

bool Level::removeDoor( ushort index )
{
    if( index >= mNumDoors ){
        return false;
    }

    mNumDoors--;

    for(ushort i = index; i < mNumDoors; i++){
        mDoors[i] = mDoors[i+1];
    }

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
    mNumFurniture = 0;
    mNumContainers = 0;
    mNumDoors = 0;
}

Level::Block& Level::getBlock( ushort i, ushort j )
{
	assert( i < mWidth );
    assert( j < mDepth );

    return mBlocks[i][j];
}

bool Level::isRectOfBlocksOpen( short l, short r, short t, short b )
{
    assert( l < mWidth );
    assert( t < mDepth );    
    assert( r < mWidth );
    assert( b < mDepth );

    //Loop through area, checking for objects
    for(short j = t; j <= b; j++){
        for(short i = l; i <= r; i++){
            //If there is a collidable object fail
            if( mBlocks[i][j].getCollidableType() != Level::Block::Collidable::None ){
                return false;
            }
        }
    }

    return true;
}

void Level::getFurnitureAABoundingSquare( Furniture& furniture , float& left, float& front, float& right, float& back )
{
    XMVECTOR pos = XMLoadFloat4( &furniture.getPosition() );

    //Set the dimensions vectors
    XMVECTOR xDim = XMVectorSet( mFurnitureDimensions[ furniture.getType() ].x / 2.0f, 0.0f, 0.0f, 0.0f );
    XMVECTOR zDim = XMVectorSet( 0.0f, 0.0f, mFurnitureDimensions[ furniture.getType() ].z / 2.0f, 0.0f );

    //Calculate each corner from the origin
    XMVECTOR frontLeft = (-xDim - zDim);
    XMVECTOR frontRight = (xDim - zDim);
    XMVECTOR backLeft = (-xDim + zDim);
    XMVECTOR backRight = (xDim + zDim);

    //Rotate and translate
    XMMATRIX transform = XMMatrixRotationY( furniture.getYRotation() ) *
                         XMMatrixTranslation( furniture.getPosition().x, 
                                              furniture.getPosition().y, 
                                              furniture.getPosition().z );

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

void Level::getContainerAABoundingSquare( Container& container , float& left, float& front, float& right, float& back )
{
    XMVECTOR pos = XMLoadFloat4( &container.getPosition() );

    //Set the dimensions vectors
    XMVECTOR xDim = XMVectorSet( mContainerDimensions[ container.getCType() ].x / 2.0f, 0.0f, 0.0f, 0.0f );
    XMVECTOR zDim = XMVectorSet( 0.0f, 0.0f, mContainerDimensions[ container.getCType() ].z / 2.0f, 0.0f );

    //Calculate each corner from the origin
    XMVECTOR frontLeft = (-xDim - zDim);
    XMVECTOR frontRight = (xDim - zDim);
    XMVECTOR backLeft = (-xDim + zDim);
    XMVECTOR backRight = (xDim + zDim);

    //Rotate and translate
    XMMATRIX transform = XMMatrixRotationY( container.getYRotation() ) *
                         XMMatrixTranslation( container.getPosition().x, 
                                              container.getPosition().y, 
                                              container.getPosition().z );

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

void Level::Door::open()
{
    if( state == Door::Closed ){
        state = Door::Opening;
        startAngle = mYRotation;
    }
}

void Level::Door::close()
{
    if( state == Door::Opened ){
        state = Door::Closing;
        startAngle = mYRotation;
    }
}