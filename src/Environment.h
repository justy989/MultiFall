#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Utils.h"

#define ENV_ROOM_MAX_HEIGHT 8
#define ENV_ROOM_MAX_EXIT_HEIGHT 3

class Environment{
public:

    //Holds a room in an environment
    class Room{
    public:

        Room();
        ~Room();

        //Holds an exit in a room
        struct Exit{
            enum Side{
                Front,
                Left,
                Back,
                Right
            };

            byte height; //How high the door is
            byte location; //The x or y location based the side the door is on
        };

        //Holds a block in a room
        struct Block{
            //Direction ramp is pointing and the height is the bottom of the ramp, is none if not a ramp
            enum RampDirection{
                None,
                Up,
                Left,
                Down,
                Right
            };

            byte ramp;
            byte height;
        };

        //initialize a room to certain dimensions
        bool init( byte width, byte depth, byte height );

        //clear the room's allocated memory
        void clear();

        //set the exit
        void setExit( Exit::Side side, byte height, byte location );

        //set a block
        void setBlock( byte i, byte j, byte height, Block::RampDirection ramp );

        //Get the ramp type of a block
        Block::RampDirection getBlockRamp( byte i, byte j );

        //Get the height of a block
        byte getBlockHeight( byte i, byte j );

        //Get the height of an exit
        byte getExitHeight( Exit::Side side );

        //Get the location of an exit
        byte getExitLocation( Exit::Side side );

        inline byte getWidth();
        inline byte getDepth();
        inline byte getHeight();

    protected:

        byte mWidth;
        byte mDepth;
        byte mHeight;

        Block** mBlocks;

        Exit mExits[4];
    };

    Environment();

    inline Room& getRoom();

protected:

    Room mRoom;
};

byte Environment::Room::getWidth(){return mWidth;}
byte Environment::Room::getDepth(){return mDepth;}
byte Environment::Room::getHeight(){return mHeight;}

inline Environment::Room& Environment::getRoom(){return mRoom;}

#endif