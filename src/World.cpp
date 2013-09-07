#include "World.h"

World::World()
{

}

void World::update( float dt )
{

}

void World::moveEntity( WorldEntity* entity, XMVECTOR moveVec, float dist )
{
    //Try to move the entity in the world along the moveVec
    XMVECTOR pos = XMLoadFloat4( &entity->getPosition() );
    XMVECTOR desiredPos = pos;
    XMVECTOR wall = XMVectorZero();

    int checks = 0;
    int maxChecks = 10;
    float saveDist = dist;
    float distInterval = dist / static_cast<float>(maxChecks);

    //Move along by the movement vector until collision is detected
    XMVECTOR vel = XMVectorScale( moveVec, dist );

    if( checkEntityCollision( entity, desiredPos, &wall ) ){
        XMVECTOR invWall = XMVectorNegate( wall );
        wall = wall * XMVector3Length( moveVec * invWall );
        vel = (moveVec - wall) * dist;
    }
    
    desiredPos = pos + vel;
    XMStoreFloat4( &entity->getPosition(), desiredPos );
}

bool World::checkEntityCollision( WorldEntity* entity, XMVECTOR desiredPosition, XMVECTOR* outWallVector)
{
    Level& room = mDungeon.getLevel(); //Reference to Room

    float px = entity->getPosition().x; //Player X
    float py = entity->getPosition().y; //Player Y
    float pz = entity->getPosition().z; //Player Z
    float pf = entity->getPosition().y - entity->getSolidity().height; //Player Feet

    int bX = static_cast<int>(px / 0.3f); //Block Starting X
    int bZ = static_cast<int>(pz / 0.3f); //Block Starting X

    //Check the left block's right side
    int i = bX - 1;
    int j = bZ;

    float blockHeight = 0.0f;

    float left = 0.0f;
    float front = 0.0f;
    float right = 0.0f;
    float back = 0.0f;

    XMVECTOR wallX = XMVectorZero();
    XMVECTOR wallZ = XMVectorZero();

    bool collided = false;

    /* TODO: Corner collision so you can't go through walls
    //Check the top Left Corner
    i = bX + 1;
    j = bZ + 1;
    
    if( i >= 0 && i < room.getWidth() &&
        j >= 0 && j < room.getDepth() ){

        blockHeight = static_cast<float>(room.getBlockHeight(i, j)) * 0.3f;

        if( blockHeight > pf ){
            if( WorldEntity::circlePointIntersect( 
                XMVectorSet(left, top, 0.0f, 1.0f),
                XMVectorSet(px, 0.0f, pz, 1.0f),
                entity->getSolidity().radius ) ){
                return true;
            }
        }
    }*/

    if( px - entity->getSolidity().radius < 0.0f ){
        wallX = XMVectorSet( -1.0f, 0.0f, 0.0f, 1.0f );
        collided = true;
    }else if( px + entity->getSolidity().radius > 0.3f * room.getWidth() ){
        wallX = XMVectorSet( 1.0f, 0.0f, 0.0f, 1.0f );
        collided = true;
    }

    if( pz - entity->getSolidity().radius < 0.0f ){
        wallZ = XMVectorSet( 0.0f, 0.0f, -1.0f, 1.0f );
        collided = true;
    }else if( pz + entity->getSolidity().radius > 0.3f * room.getDepth() ){
        wallZ = XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f );
        collided = true;
    }

    //Check the left Block's right side
    i = bX - 1;
    j = bZ;

    if( i >= 0 && i < room.getWidth() &&
        j >= 0 && j < room.getDepth() ){

        blockHeight = static_cast<float>(room.getBlockHeight(i, j)) * 0.3f;

        if( blockHeight > pf ){
            left = static_cast<float>(i) * 0.3f;
            front = static_cast<float>(j) * 0.3f;
            right = left + 0.3f;
            back = front + 0.3f;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(right, 0.0f, front, 1.0f),
                            XMVectorSet(right, 0.0f, back, 1.0f),
                            XMVectorSet(px, 0.0f, pz, 1.0f),
                            entity->getSolidity().radius ) ){
                wallX = XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f );
                collided = true;
            }
        }
    }

    //Check the right block' left side
    i = bX + 1;
    j = bZ;

    if( i >= 0 && i < room.getWidth() &&
        j >= 0 && j < room.getDepth() ){

        blockHeight = static_cast<float>(room.getBlockHeight(i, j)) * 0.3f;

        if( blockHeight > pf ){
            left = static_cast<float>(i) * 0.3f;
            front = static_cast<float>(j) * 0.3f;
            right = left + 0.3f;
            back = front + 0.3f;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(left, 0.0f, front, 1.0f),
                            XMVectorSet(left, 0.0f, back, 1.0f),
                            XMVectorSet(px, 0.0f, pz, 1.0f),
                            entity->getSolidity().radius ) ){
                wallX = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f );
                collided = true;
            }
        }
    }
    
    //Check the front block's back side
    i = bX;
    j = bZ - 1;

    if( i >= 0 && i < room.getWidth() &&
        j >= 0 && j < room.getDepth() ){

        blockHeight = static_cast<float>(room.getBlockHeight(i, j)) * 0.3f;

        if( blockHeight > pf ){
            left = static_cast<float>(i) * 0.3f;
            front = static_cast<float>(j) * 0.3f;
            right = left + 0.3f;
            back = front + 0.3f;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(left, 0.0f, back, 1.0f),
                            XMVectorSet(right, 0.0f, back, 1.0f),
                            XMVectorSet(px, 0.0f, pz, 1.0f),
                            entity->getSolidity().radius ) ){
                wallZ = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f );
                collided = true;
            }
        }
    }
    
    //Check the back block's front side
    i = bX;
    j = bZ + 1;

    if( i >= 0 && i < room.getWidth() &&
        j >= 0 && j < room.getDepth() ){

        blockHeight = static_cast<float>(room.getBlockHeight(i, j)) * 0.3f;

        if( blockHeight > pf ){
            left = static_cast<float>(i) * 0.3f;
            front = static_cast<float>(j) * 0.3f;
            right = left + 0.3f;
            back = front + 0.3f;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(left, 0.0f, front, 1.0f),
                            XMVectorSet(right, 0.0f, front, 1.0f),
                            XMVectorSet(px, 0.0f, pz, 1.0f),
                            entity->getSolidity().radius ) ){
                wallZ = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f );
                collided = true;
            }
        }
    }

    *outWallVector = wallX + wallZ;
    return collided;
}