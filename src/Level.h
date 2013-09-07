#ifndef LEVEL_H
#define LEVEL_H

#include "Utils.h"

//Holds a level of a Dungeon
class Level{
public:

    Level();
    ~Level();

    //Holds a block in a room
    struct Block{
        //Direction ramp is pointing and the height is the bottom of the ramp, is none if not a ramp
        enum Ramp{
            None,
            Front,
            Left,
            Back,
            Right
        };

        byte ramp; //is this a ramp? If so, what kind?
        byte height; //Height of the Level at this block
        byte tileId; //ID for tiling
        byte wallID; //ID wall texture selection
    };

    //initialize a room to certain dimensions
    bool init( byte width, byte depth, byte height );

    //clear the room's allocated memory
    void clear();

    //set a block
    void setBlock( byte i, byte j, byte height, Block::Ramp ramp );

	//set block ID (used for spritesheet lookup)
	void setBlockTileID( byte i, byte j, byte id );

	//get block ID (used for spritesheet lookup)
	byte getBlockTileID( byte i, byte j );		

    //Get the ramp type of a block
    Block::Ramp getBlockRamp( byte i, byte j );

    //Get the height of a block
    byte getBlockHeight( byte i, byte j );

    inline byte getWidth();
    inline byte getDepth();
    inline byte getHeight();

protected:

    byte mWidth;
    byte mDepth;
    byte mHeight;

    Block** mBlocks;
};

byte Level::getWidth(){return mWidth;}
byte Level::getDepth(){return mDepth;}
byte Level::getHeight(){return mHeight;}

#endif