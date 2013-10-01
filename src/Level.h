#ifndef LEVEL_H
#define LEVEL_H

#include "Utils.h"
#include "Light.h"
#include "StaticMesh.h"

#define LEVEL_MAX_LIGHTS 128
#define LEVEL_MAX_FURNITURE 1024
#define LEVEL_FURNITURE_TYPE_COUNT 7

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

    //Holds a block in a room
    struct Block{
        byte ramp; //is this a ramp? If so, what kind?
        byte height; //Height of the Level at this block
        byte tileId; //ID for tiling
        byte wallID; //ID wall texture selection
    };

	struct TorchInfo
	{
		XMMATRIX world;
		ushort mesh;
		//potentially other stuff like texture maybe
	};

    struct Furniture{
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
        XMFLOAT4 position;
        float yRotation;
    };

    //initialize a room to certain dimensions
    bool init( short width, short depth, byte height );

    //Add a light if there is room for it!
    bool addLight( PointLight& light );

    //Add a torch
	bool addTorch( XMFLOAT3 pos, float rotAbootYAxis );

    //Add a piece of furniture
    bool addFurniture( Furniture::Type type, XMFLOAT4 position, float yRot );

    //clear the room's allocated memory
    void clear();

    //set a block
    void setBlock( short i, short j, byte height, Ramp ramp );

	//set block ID (used for spritesheet lookup)
	void setBlockTileID( short i, short j, byte id );

	//get block ID (used for spritesheet lookup)
	byte getBlockTileID( short i, short j );		

    //Get the ramp type of a block
    Ramp getBlockRamp( short i, short j );

    //Get the height of a block
    byte getBlockHeight( short i, short j );

    //Is a rectangle of blocks the same height?
    bool isRectOfBlocksSameHeight( short l, short r, short t, short b, byte height );

    //From furniture passed in, get a bounding box
    void getFurnitureAABoundingSquare( Furniture& furniture , float& left, float& front, float& right, float& back );

    inline short getWidth();
    inline short getDepth();
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
    short mWidth;
    short mDepth;
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

short Level::getWidth(){return mWidth;}
short Level::getDepth(){return mDepth;}
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