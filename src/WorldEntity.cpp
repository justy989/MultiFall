#include "WorldEntity.h"

WorldEntity::WorldEntity() :
    mExistence(Dead),
    mID( -1 )
{
    mPosition.x = 0;
    mPosition.y = 0;
    mPosition.z = 0;
}