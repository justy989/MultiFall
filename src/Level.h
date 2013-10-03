#ifndef LEVEL_H
#define LEVEL_H

#include "Utils.h"
#include "Light.h"
#include "StaticMesh.h"

#define LEVEL_MAX_LIGHTS 128
#define LEVEL_MAX_FURNITURE 1024
#define LEVEL_FURNITURE_TYPE_COUNT 7
#define LEVEL_LIGHT_TYPE_COUNT 4

//Holds a level of a Dungeon
class Level{
public:

    Level();
    ~Level();

    //Direction ramp is pointing and the height is the bottom of the ramp, is none if not a ramp
    enum Ramp{
        None,
        Front,
        Left,
        Back,
        Right
    };

    //Solid Object in a Level
    class SolidObject{
    public:
        XMFLOAT4 position;
        float yRotation;
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

        Type type;
    };

    //Transition between rooms
    class Door : public SolidObject{
    public:

        enum State{
            Closed,
            Opening,
            Opened
        };

        State state;
    };

    //Container holding items
    class Container : public SolidObject{
    public:

        enum State{
            Closed,
            Opening,
            Opened
        };

        enum Type{
            None,
            Crate,
            Barrel,
            Chest,
        };

        State state;
        Type type;
    };

    //Holds a block of floor in the level
    class Block{
    public:

        //Stores what type of collidable object might be on top of the floor
        enum Collidable{
            None,
            Ramp,
            Door,
            Wall,
            Furniture,
            Container,
        };

        Block();

        inline void setHeight( byte height );

        inline void setTileID( byte id );
        inline void setWallID( byte id );

        inline void setOpen( );
        inline void setRamp( Level::Ramp ramp );
        inline void setWall( byte height );
        inline void setDoor( Level::Door* door );
        inline void setFurniture( Level::Furniture* furniture );
        inline void setContainer( Level::Container* container );

        inline byte getHeight();
        inline byte getTileID();
        inline byte getWallID();

        inline Collidable getCollidableType();

        inline Level::Ramp getRamp();
        inline Level::Door* getDoor();
        inline Level::Furniture* getFurniture();
        inline Level::Container* getContainer();

    protected:

        byte mCollidable; //is there a collidable thing on this block? If so, what is it?
        byte mHeight; //Height of the floor at this block
        byte mTileID; //ID for tile texture clipping
        byte mWallID; //ID for wall texture

        //Based on the collidable, use the union to access the type of thing to check collision with
        union{
            Level::Furniture* mFurniture;
            Level::Door* mDoor;
            Level::Container* mContainer;
            Level::Ramp mRamp;
        };
    };

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

        inline void set( ushort i, ushort j, ushort height, Type type, AttachedWall = Front );

        inline Type getType();

        inline AttachedWall getAttachedWall();

        inline ushort getI();
        inline ushort getJ();

        inline ushort getHeight();

    protected:

        //Type of light
        Type mType;

        //Block position
        ushort mI;
        ushort mJ;

        //height
        ushort mHeight;

        //Which wall it is attached to, if it is a torch
        AttachedWall mAttachedWall;
    };

	struct TorchInfo
	{
		XMMATRIX world;
		ushort mesh;
		//potentially other stuff like texture maybe
	};

    //initialize a room to certain dimensions
    bool init( short width, short depth, byte height );

    //Add a light if there is room for it!
    bool addLight( PointLight& light );

    //Add a torch
	bool addTorch( XMFLOAT3 pos, float rotAbootYAxis );

    //Add a piece of furniture
    ushort addFurniture( Furniture::Type type, XMFLOAT4 position, float yRot );
    ushort addFurniture( Furniture& furniture );

    //Remove furniture
    bool removeFurniture( ushort index );

    //clear the room's allocated memory
    void clear();

    //get a block to set it
    Block& getBlock( ushort i, ushort j );

    //Is a rectangle of blocks the same height?
    bool isRectOfBlocksLevelAndOpen( short l, short r, short t, short b );

    //From furniture passed in, get a bounding box
    void getFurnitureAABoundingSquare( Furniture& furniture , float& left, float& front, float& right, float& back );

    inline ushort getWidth();
    inline ushort getDepth();
    inline byte getHeight();

    inline ushort getNumLights();
    inline PointLight& getLight( ushort index );

	inline ushort getNumTorches();
	inline TorchInfo& getTorch( ushort index );
    
    inline ushort getNumFurniture();
	inline Furniture& getFurniture( ushort index );

    inline XMFLOAT3& getFurnitureDimensions( Furniture::Type type );
    inline void setFurnitureDimensions( Furniture::Type type, XMFLOAT3 dimensions );

protected:

    //Environment Layout
    ushort mWidth;
    ushort mDepth;
    byte mHeight;

    Block** mBlocks;

    //Lights
    ushort mNumLights;
    PointLight mLights[ LEVEL_MAX_LIGHTS ];

    //Furniture
	ushort mNumTorches;
	TorchInfo mTorches[ LEVEL_MAX_LIGHTS ];

    ushort mNumFurniture;
    Furniture mFurniture[ LEVEL_MAX_FURNITURE ];

    //Dimensions of furniture by type
    XMFLOAT3 mFurnitureDimensions[ LEVEL_FURNITURE_TYPE_COUNT ];
};

//Block Inline Functions
inline void Level::Block::setHeight( byte height ){mHeight = height;}

inline void Level::Block::setTileID( byte id ){mTileID = id;}
inline void Level::Block::setWallID( byte id ){mWallID = id;}

inline void Level::Block::setOpen( ){mCollidable = Level::Block::Collidable::None;}
inline void Level::Block::setRamp( Level::Ramp ramp ){mCollidable = Level::Block::Collidable::Ramp; mRamp = ramp;}
inline void Level::Block::setWall( byte height ){mCollidable = Level::Block::Collidable::Wall; mHeight = height;}
inline void Level::Block::setDoor( Level::Door* door ){mCollidable = Level::Block::Collidable::Door; mDoor = door;}
inline void Level::Block::setFurniture( Level::Furniture* furniture ){mCollidable = Level::Block::Collidable::Furniture; mFurniture = furniture;}
inline void Level::Block::setContainer( Level::Container* container ){mCollidable = Level::Block::Collidable::Container; mContainer = container;}

inline Level::Block::Collidable Level::Block::getCollidableType(){return (Level::Block::Collidable)(mCollidable);}

inline Level::Ramp Level::Block::getRamp(){return mRamp;}
inline Level::Door* Level::Block::getDoor(){return mDoor;}
inline Level::Furniture* Level::Block::getFurniture(){return mFurniture;}
inline Level::Container* Level::Block::getContainer(){return mContainer;}

inline byte Level::Block::getHeight(){return mHeight;}
inline byte Level::Block::getTileID(){return mTileID;}
inline byte Level::Block::getWallID(){return mWallID;}

//Light Inline Functions
inline void Level::Light::set( ushort i, ushort j, ushort height, Type type, Level::Light::AttachedWall attachedWall )
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

inline ushort Level::Light::getHeight(){return mHeight;}

//Level Inline Functions
ushort Level::getWidth(){return mWidth;}
ushort Level::getDepth(){return mDepth;}
byte Level::getHeight(){return mHeight;}

inline ushort Level::getNumLights(){return mNumLights;}
inline PointLight& Level::getLight( ushort index ){return mLights[ index ];}

inline ushort Level::getNumTorches(){return mNumTorches;}
inline Level::TorchInfo& Level::getTorch( ushort index ){return mTorches[ index ];}

inline ushort Level::getNumFurniture(){return mNumFurniture;}
inline Level::Furniture& Level::getFurniture( ushort index ){return mFurniture[ index ];}

inline XMFLOAT3& Level::getFurnitureDimensions( Level::Furniture::Type type ){return mFurnitureDimensions[type];}
inline void Level::setFurnitureDimensions( Level::Furniture::Type type, XMFLOAT3 dimensions ){mFurnitureDimensions[type] = dimensions;}

#endif