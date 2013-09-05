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

bool WorldEntity::circleLineIntersect( XMFLOAT2 startPoint, XMFLOAT2 endPoint, XMFLOAT2 circleCenter, float circleRadius )
{
    //Taken from: http://blog.csharphelper.com/2010/03/28/determine-where-a-line-intersects-a-circle-in-c.aspx
    float dx, dy, A, B, C, det, t;

    dx = endPoint.x - startPoint.x;
    dy = endPoint.y - startPoint.y;

    A = dx * dx + dy * dy;
    B = 2 * (dx * (startPoint.x - circleCenter.x) + dy * (startPoint.y - circleCenter.y));
    C = (startPoint.x - circleCenter.x) * (startPoint.x - circleCenter.x) + (startPoint.y - circleCenter.y) * (startPoint.y - circleCenter.y) - circleRadius * circleRadius;

    det = B * B - 4 * A * C;

    if ( A <= 0.0000001f || det < 0 )
    {
        return false;
    }
    else if (det == 0)
    {
        return true;
    }
    else
    {
        // Two solutions.
        return true;
    }

    return false;
}