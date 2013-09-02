#include "WorldGenerator.h"

#include <time.h>

WorldGenerator::WorldGenerator()
{
    mRand.seed( time( 0 ) );
}

void WorldGenerator::genRoom( Environment::Room& room )
{
    for(byte i = 0; i < room.getWidth(); i++){
        for( byte j = 0; j < room.getDepth(); j++){
            room.setBlock( i, j, mRand.gen( 0, 3 ), Environment::Room::Block::RampDirection::None );
        }
    }
}