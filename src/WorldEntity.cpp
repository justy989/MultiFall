#include "WorldEntity.h"

WorldEntity::WorldEntity() :
    mExistence(Dead),
    mID( -1 )
{
    mPosition.x = 0;
    mPosition.y = 0;
    mPosition.z = 0;
    mPosition.w = 0;
}

bool WorldEntity::circleAALineIntersect( XMVECTOR start, XMVECTOR end, XMVECTOR circleCenter, float circleRadius )
{
    //A collision function we actually understand...
 
    //Create a vector from the start to the circle position
    XMVECTOR cToStart = circleCenter - start;
    XMVECTOR cToEnd = circleCenter - end;

    XMVECTOR lenToStart = XMVector4Length( cToStart );
    XMVECTOR lenToEnd = XMVector4Length( cToEnd );

    XMFLOAT4 ans;
    XMStoreFloat4( &ans, lenToStart );

    if( ans.x <= circleRadius ){
        return true;
    }

    XMStoreFloat4( &ans, lenToEnd );

    if( ans.x <= circleRadius ){
        return true;
    }

    //Calculate the start to end
    XMVECTOR endToStart = end - start;

    XMVECTOR endToStartLen = XMVector4Length( endToStart );
        
    XMFLOAT4 tmp;
    XMStoreFloat4(&tmp, endToStartLen);
    tmp.x = 1.0f / tmp.x;

    XMVECTOR tmpA = XMVectorScale(endToStart, tmp.x);
    XMVECTOR tmpB = XMVectorScale(cToStart, tmp.x);

    //Project it onto the start -> end vector
    XMVECTOR dot = XMVector4Dot( tmpA, tmpB );

    //Calculate the Perpendicular point
    XMVECTOR per = start + ( endToStart * dot );

    //If the perpendicular point is within range of the circle, there is a collision
    XMVECTOR perDist = XMVector4Length( circleCenter - per );

    XMStoreFloat4( &ans, perDist );

    if( ans.x > circleRadius ){
        return false;
    }
    
    //Store the dot product to see how far down the line the collision happens
    XMStoreFloat4( &ans, dot );

    //If the dot product is negative, the collision is outside the range
    if( ans.x < 0.0f ){
        return false;
    }

    //If the dot product is bigger than 1.0f, meaning it scaled the vector passed the segment, the collision is outside the range
    if( ans.x > 1.0f ){
        return false;
    }

    return true;
}