#ifndef LEVEL_H
#define LEVEL_H

#include "Utils.h"
#include "Light.h"
#include "StaticMesh.h"
#include "WorldContainer.h"

#define LEVEL_MAX_LIGHTS 128
#define LEVEL_MAX_FURNITURE 512
#define LEVEL_MAX_CONTAINERS 512
#define LEVEL_MAX_DOORS 128
#define LEVEL_FURNITURE_TYPE_COUNT 7
#define LEVEL_LIGHT_TYPE_COUNT 4

//Holds a level of a Dungeon
class Level{
public:

    Level();
    ~Level();

    enum Direction{
        Front,
        Left,
        Back,
        Right
    };

    //Solid Object in a Level
    class SolidObject{
    public:

        inline void setYRotation( float f );

        inline XMFLOAT4& getPosition();
        inline float getYRotation();

    protected:

        XMFLOAT4 mPosition;
        float mYRotation;
    };

    //Furniture placed in the dungeon
    class Furniture : public SolidObject{
    public:

        enum Type{
            None,
            Chair,
            Desk,
            Table,
            Bench,
            Bed_Frame,
            Book_Case
        };

        inline void setType( Type type );
        inline Type getType( );

    protected:

        Type mType;
    };

    //Doesn't need anything special, just these two plopped together
    class Container : public WorldContainer, public SolidObject{

    };

    //Transition between rooms
    class Door : public SolidObject{
    public:

        enum State{
            Closed,
            Opening,
            Opened,
            Closing
        };

        void open();
        void close();

        State state;
        float openInterval;
        Direction facing;

        float startAngle;
    };

    //Lights in a level
    class Light{
    public:

        enum Type{
            None,
            Candle,
            Torch,
            Chandelier
        };

        enum AttachedWall{
            Front,
            Left,
            Right,
            Back
        };

        inline void set( ushort i, ushort j, float height, Type type, AttachedWall = Front );

        inline Type getType();

        inline AttachedWall getAttachedWall();

        inline ushort getI();
        inline ushort getJ();

        inline float getHeight();

    protected:

        //Type of light
        Type mType;

        //Block position
        ushort mI;
        ushort mJ;

        //height
        float mHeight;

        //Which wall it is attached to, if it is a torch
        AttachedWall mAttachedWall;
    };

    //Holds a block of floor in the level
    class Block{
    public:

        //Stores what type of collidable object might be on top of the floor
        enum Collidable{
            None,
            Door,
            Wall,
            Furniture,
            Container,
        };

        Block();

        inline void setTileID( byte id );
        inline void setWallID( byte id );

        inline void setOpen( );
        inline void setWall( );
        inline void setDoor( Level::Door* door );
        inline void setFurniture( Level::Furniture* furniture );
        inline void setContainer( Level::Container* container );

        inline byte getTileID();
        inline byte getWallID();

        inline Collidable getCollidableType();

        inline Level::Door* getDoor();
        inline Level::Furniture* getFurniture();
        inline Level::Container* getContainer();

    protected:

        byte mCollidable; //is there a collidable thing on this block? If so, what is it?
        byte mTileID; //ID for tile texture clipping
        ushort mPadding; //Not used for anything atm

        //Based on the collidable, use the union to access the type of thing to check collision with
        union{
            Level::Furniture* mFurniture;
            Level::Door* mDoor;
            Level::Container* mContainer;
        };
    };

    //initialize a room to certain dimensions
    bool init( short width, short depth, byte height );

    //Add a light if there is room for it!
    bool addLight( Light& light );

    //Add a piece of furniture
    ushort addFurniture( Furniture::Type type, XMFLOAT4 position, float yRot );
    ushort addFurniture( Furniture& furniture );

    //Remove furniture
    bool removeFurniture( ushort index );

    //Add a container
    ushort addContainer( Level::Container& container );

    //Remove a container
    bool removeContainer( ushort index );

    //Add a container
    ushort addDoor( Door& door );

    //Remove a container
    bool removeDoor( ushort index );

    //clear the room's allocated memory
    void clear();

    //get a block to set it
    Block& getBlock( ushort i, ushort j );

    //Is a rectangle of blocks the same height?
    bool isRectOfBlocksOpen( short l, short r, short t, short b );

    //From furniture passed in, get a bounding box
    void getFurnitureAABoundingSquare( Furniture& furniture , float& left, float& front, float& right, float& back );

    //From container passed in, get a bounding box
    void getContainerAABoundingSquare( Container& container , float& left, float& front, float& right, float& back );

    //From a door passed in, get the bounding box
    void getDoorAABoundingSquare( Door& door, float& left, float& front, float& right, float& back );

    inline ushort getWidth();
    inline ushort getDepth();
    inline byte getHeight();

    inline ushort getNumLights();
    inline Light& getLight( ushort index );
    
    inline ushort getNumFurniture();
	inline Furniture& getFurniture( ushort index );

    inline ushort getNumContainer();
	inline Container& getContainer( ushort index );

    inline short getNumDoors();
    inline Door& getDoor( ushort index );

    inline XMFLOAT3& getFurnitureDimensions( Furniture::Type type );
    inline void setFurnitureDimensions( Furniture::Type type, XMFLOAT3 dimensions );

    inline XMFLOAT3& getContainerDimensions( Container::Type type );
    inline void setContainerDimensions( Container::Type type, XMFLOAT3 dimensions );

    inline XMFLOAT3& getDoorDimensions( );
    inline void setDoorDimensions( XMFLOAT3 dimensions );

protected:

    //Environment Layout
    ushort mWidth;
    ushort mDepth;
    byte mHeight;

    //2 dimensional array of blocks
    Block** mBlocks;

    //Lights
    ushort mNumLights;
    Light mLights[ LEVEL_MAX_LIGHTS ];

    //Furniture
    ushort mNumFurniture;
    Furniture mFurniture[ LEVEL_MAX_FURNITURE ];

    //Containers
    ushort mNumContainers;
    Level::Container mContainers[ LEVEL_MAX_CONTAINERS ];

    //Doors
    ushort mNumDoors;
    Door mDoors[ LEVEL_MAX_DOORS ];

    //Dimensions of furniture by type
    XMFLOAT3 mFurnitureDimensions[ LEVEL_FURNITURE_TYPE_COUNT ];
    XMFLOAT3 mContainerDimensions[ WORLD_CONTAINER_TYPE_COUNT ];
    XMFLOAT3 mDoorDimensions;
};

//Block Inline Functions
inline void Level::Block::setTileID( byte id ){mTileID = id;}
//inline void Level::Block::setWallID( byte id ){mWallID = id;}

inline void Level::Block::setOpen( ){mCollidable = Level::Block::Collidable::None; mFurniture = NULL;}
inline void Level::Block::setWall( ){mCollidable = Level::Block::Collidable::Wall;}
inline void Level::Block::setDoor( Level::Door* door ){mCollidable = Level::Block::Collidable::Door; mDoor = door; }
inline void Level::Block::setFurniture( Level::Furniture* furniture ){mCollidable = Level::Block::Collidable::Furniture; mFurniture = furniture;}
inline void Level::Block::setContainer( Level::Container* container ){mCollidable = Level::Block::Collidable::Container; mContainer = container;}

inline Level::Block::Collidable Level::Block::getCollidableType(){return (Level::Block::Collidable)(mCollidable);}

inline Level::Door* Level::Block::getDoor(){return mDoor;}
inline Level::Furniture* Level::Block::getFurniture(){return mFurniture;}
inline Level::Container* Level::Block::getContainer(){return mContainer;}

inline byte Level::Block::getTileID(){return mTileID;}
//inline byte Level::Block::getWallID(){return mWallID;}

//Light Inline Functions
inline void Level::Light::set( ushort i, ushort j, float height, Type type, Level::Light::AttachedWall attachedWall )
{
    mI = i;
    mJ = j;

    mHeight = height;

    mType = type;

    mAttachedWall = attachedWall;
}

inline Level::Light::Type Level::Light::getType(){return mType;}

inline Level::Light::AttachedWall Level::Light::getAttachedWall(){return mAttachedWall;}

inline ushort Level::Light::getI(){return mI;}
inline ushort Level::Light::getJ(){return mJ;}

inline float Level::Light::getHeight(){return mHeight;}

//Solid Object Index Functions
inline void Level::SolidObject::setYRotation( float f ){mYRotation = f;}

inline XMFLOAT4& Level::SolidObject::getPosition(){return mPosition;}
inline float Level::SolidObject::getYRotation(){return mYRotation;}

//Level Inline Functions
ushort Level::getWidth(){return mWidth;}
ushort Level::getDepth(){return mDepth;}
byte Level::getHeight(){return mHeight;}

inline ushort Level::getNumLights(){return mNumLights;}
inline Level::Light& Level::getLight( ushort index ){return mLights[ index ];}

inline ushort Level::getNumFurniture(){return mNumFurniture;}
inline Level::Furniture& Level::getFurniture( ushort index ){return mFurniture[ index ];}

inline ushort Level::getNumContainer(){return mNumContainers;}
inline Level::Container& Level::getContainer( ushort index ){return mContainers[ index ];}

inline short Level::getNumDoors(){return mNumDoors;}
inline Level::Door& Level::getDoor( ushort index ){return mDoors[index];}

inline XMFLOAT3& Level::getFurnitureDimensions( Level::Furniture::Type type ){return mFurnitureDimensions[type];}
inline void Level::setFurnitureDimensions( Level::Furniture::Type type, XMFLOAT3 dimensions ){mFurnitureDimensions[type] = dimensions;}

inline XMFLOAT3& Level::getContainerDimensions( Level::Container::Type type ){return mContainerDimensions[type];}
inline void Level::setContainerDimensions( Level::Container::Type type, XMFLOAT3 dimensions ){mContainerDimensions[type] = dimensions;}

inline XMFLOAT3& Level::getDoorDimensions( ){return mDoorDimensions;}
inline void Level::setDoorDimensions( XMFLOAT3 dimensions ){mDoorDimensions = dimensions;}

//Furniture inline functiosn
inline void Level::Furniture::setType( Type type ){mType = type;}
inline Level::Furniture::Type Level::Furniture::getType( ){return mType;}

#endif