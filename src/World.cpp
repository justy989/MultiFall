#include "World.h"

World::World()
{

}

void World::update( float dt )
{
    //Update the population, mostly does AI decisions
    mPop.update( dt );

    float mBlockDimensions = 0.3f;

    //Do collision for moving characters
    for( int i = 0; i < POPULATION_MAX_CHARACTERS; i++){
        Character& character = mPop.getCharacter(i);

        if( character.getExistence() > Character::Existence::Dead ){
            XMFLOAT4& dir = mPop.getCharacter(i).getWalkingDirection();

            //Make sure we are at least moving when checking collision
            float dist = fabs( dir.x ) + fabs( dir.z );
            if( dist < 0.1f ){
                continue;
            }

            //TEMPORARY WAY OF SOLVING THIS!
            //As long as we are not on a ramp, bring us to the floor
            int bx = static_cast<int>( character.getPosition().x / mBlockDimensions );
            int bz = static_cast<int>( character.getPosition().z / mBlockDimensions );

            CLAMP( bx, 0, getLevel().getWidth() - 1 );
            CLAMP( bz, 0, getLevel().getDepth() - 1 );

            if( getLevel().getBlock(bx, bz).getCollidableType() == Level::Block::Collidable::None ){
                //Make sure we are on the floor, otherwise bring us down through gravity
                float distFromGround = ( character.getPosition().y - character.getSolidity().height ) - static_cast<float>(getLevel().getBlock(bx, bz).getHeight()) * mBlockDimensions;

                if( distFromGround > 0.05f ){
                    dir.y = 1.0f;
                }else if(distFromGround < -0.05f ){
                    dir.y = 1.0f;
                }
            }else if( getLevel().getBlock(bx, bz).getCollidableType() == Level::Block::Collidable::Ramp ){
                if( getLevel().getBlock(bx, bz).getRamp() == Level::Ramp::Front ){
                    float blockHeight = static_cast<float>(getLevel().getBlock(bx, bz).getHeight()) * mBlockDimensions;
                    float rampUp = fmod(character.getPosition().z, mBlockDimensions);
                    rampUp *= 1.25f;
                    character.getPosition().y = blockHeight + rampUp + character.getSolidity().height;
                }else if( getLevel().getBlock(bx, bz).getRamp() == Level::Ramp::Back ){
                    float blockHeight = static_cast<float>(getLevel().getBlock(bx, bz).getHeight()) * mBlockDimensions;
                    float rampUp = fmod(character.getPosition().z, mBlockDimensions);
                    rampUp = (mBlockDimensions - rampUp) * 1.25f;
                    character.getPosition().y = blockHeight + rampUp + character.getSolidity().height;
                }else if( getLevel().getBlock(bx, bz).getRamp() == Level::Ramp::Left ){
                    float blockHeight = static_cast<float>(getLevel().getBlock(bx, bz).getHeight()) * mBlockDimensions;
                    float rampUp = fmod(character.getPosition().x, mBlockDimensions);
                    rampUp *= 1.25f;
                    character.getPosition().y = blockHeight + rampUp + character.getSolidity().height;
                }else if( getLevel().getBlock(bx, bz).getRamp() == Level::Ramp::Right ){
                    float blockHeight = static_cast<float>(getLevel().getBlock(bx, bz).getHeight()) * mBlockDimensions;
                    float rampUp = fmod(character.getPosition().x, mBlockDimensions);
                    rampUp = (mBlockDimensions - rampUp) * 1.25f;
                    character.getPosition().y = blockHeight + rampUp + character.getSolidity().height;
                }
            }

            XMVECTOR moveVec = XMLoadFloat4( &dir );

            moveVec = XMVector3Normalize( moveVec );
            moveVec = XMVectorSetW( moveVec, 1.0f );

            moveEntity( &character, moveVec, dt * CHARACTER_MOVE_SPEED );

            //Reset the direction back to 0
            dir.x = 0.0f;
            dir.y = 0.0f;
            dir.z = 0.0f;
        }
    }
}

void World::handleEvent( Event& e )
{
    switch( e.type ){
    case Event::Type::CharacterSpawn:
        {
            float tx = e.characterSpawnInfo.x;
            float tz = e.characterSpawnInfo.z;
            float ty = 0.0f;

            uint tix = static_cast<uint>( tx / 0.3f );
            uint tiz = static_cast<uint>( tz / 0.3f );

            if( tix < mLevel.getWidth() &&
                tiz < mLevel.getDepth() ){
                ty = static_cast<float>( mLevel.getBlock(tix, tiz).getHeight() ) * 0.3f + 0.25f;
            }

            mPop.spawn( e.characterSpawnInfo.id, 
            XMFLOAT4( tx,
                ty,
                tz,
                1.0f ) );
        }
        break;
    case Event::Type::CharacterKill:
        mPop.kill( e.characterKillInfo.id );
        break;
    case Event::Type::CharacterWalk:
        break;
    default:
        break;
    }
}

void World::moveEntity( WorldEntity* entity, XMVECTOR moveVec, float dist )
{
    //Try to move the entity in the world along the moveVec
    XMVECTOR pos = XMLoadFloat4( &entity->getPosition() );
    XMVECTOR wall = XMVectorZero();
    XMVECTOR vel = XMVectorScale( moveVec, dist );
    XMVECTOR desiredPos;
    XMFLOAT4 check;
    XMFLOAT4 negativeCheck;

    int checks = 0;
    int maxChecks = 10;
    float saveDist = dist;
    float distInterval = dist / static_cast<float>(maxChecks);

    XMStoreFloat4( &check, XMVector3Length(moveVec) );

    //Don't bother doing anything if there is no movement direction
    if( check.x <= 0.0f ){
        return;
    }

    //Check collision at where we plan to be
    if( checkEntityCollision( entity, pos, &wall ) ){
        XMStoreFloat4( &check, wall );
        XMStoreFloat4( &negativeCheck, vel );

        //Check the negative bit of the x and z values and see if they are equal, if they are, then stop movement
        bool negativeXWall = *(int*)(&check.x) < 0;
        bool negativeZWall = *(int*)(&check.z) < 0;

        bool negativeXVel = *(int*)(&negativeCheck.x) < 0;
        bool negativeZVel = *(int*)(&negativeCheck.z) < 0;

        //If we are not in a corner, collide with the wall, otherwise stop movement
        if(check.w <= 1.5f ||
           ( negativeXWall != negativeXVel || 
             negativeZWall != negativeZVel ) ){
            XMVECTOR invWall = XMVectorNegate( wall );
            wall = wall * XMVector3Length( moveVec * invWall );
            vel = (moveVec - wall) * dist;
        }else{
            vel = XMVectorZero();
        }
    }

    desiredPos = pos + vel;
    
    XMStoreFloat4( &entity->getPosition(), desiredPos );
}

bool World::checkEntityCollision( WorldEntity* entity, XMVECTOR desiredPosition, XMVECTOR* outWallVector)
{
    float px = entity->getPosition().x; //Player X
    float py = entity->getPosition().y; //Player Y
    float pz = entity->getPosition().z; //Player Z
    float pf = entity->getPosition().y - ( entity->getSolidity().height - 0.15f); //Player Feet, take into account the tops of ramps

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
    
    if( i >= 0 && i < mLevel.getWidth() &&
        j >= 0 && j < mLevel.getDepth() ){

        blockHeight = static_cast<float>(mLevel.getBlockHeight(i, j)) * 0.3f;

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
    }else if( px + entity->getSolidity().radius > 0.3f * mLevel.getWidth() ){
        wallX = XMVectorSet( 1.0f, 0.0f, 0.0f, 1.0f );
        collided = true;
    }

    if( pz - entity->getSolidity().radius < 0.0f ){
        wallZ = XMVectorSet( 0.0f, 0.0f, -1.0f, 1.0f );
        collided = true;
    }else if( pz + entity->getSolidity().radius > 0.3f * mLevel.getDepth() ){
        wallZ = XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f );
        collided = true;
    }

    //Check the left Block's right side
    i = bX - 1;
    j = bZ;

    if( i >= 0 && i < mLevel.getWidth() &&
        j >= 0 && j < mLevel.getDepth() ){

        blockHeight = static_cast<float>( mLevel.getBlock(i, j).getHeight() ) * 0.3f;

        //Check wall
        if( blockHeight > pf ||
            ( fabs( (pf - 0.15f) - blockHeight ) < 0.05f &&
            mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Ramp &&
            ( mLevel.getBlock(i,j).getRamp() == Level::Ramp::Front ||
               mLevel.getBlock(i,j).getRamp() == Level::Ramp::Back ) ) ){
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

        //Check furniture
        if( Level::Furniture* f = mLevel.getBlock(i, j).getFurniture() ){

            float front = -mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
            float left = -mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
            float right = mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
            float back = mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;

            //If we have rotated 90 or 270 degrees, swap width and height
            if( ( f->getYRotation() > (3.14159f * 0.5f) - 0.1f &&
                  f->getYRotation() < (3.14159f * 0.5f) + 0.1f ) ||
                ( f->getYRotation() > (3.14159f * 1.5f) - 0.1f &&
                  f->getYRotation() < (3.14159f * 1.5f) + 0.1f ) ){
                float tmpL = left;
                float tmpR = right;
                left = front;
                right = back;
                front = tmpL;
                back = tmpR;
            }

            front += f->getPosition().z;
            back += f->getPosition().z;
            left += f->getPosition().x;
            right += f->getPosition().x;

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

    if( i >= 0 && i < mLevel.getWidth() &&
        j >= 0 && j < mLevel.getDepth() ){

        blockHeight = static_cast<float>(mLevel.getBlock(i, j).getHeight() ) * 0.3f;

        if( blockHeight > pf ||
            ( fabs( (pf - 0.15f) - blockHeight ) < 0.05f &&
            mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Ramp &&
            ( mLevel.getBlock(i,j).getRamp() == Level::Ramp::Front ||
               mLevel.getBlock(i,j).getRamp() == Level::Ramp::Back ) ) ){
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

        //Check furniture
        if( Level::Furniture* f = mLevel.getBlock(i, j).getFurniture() ){

            float front = -mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
            float left = -mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
            float right = mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
            float back = mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;

            //If we have rotated 90 or 270 degrees, swap width and height
            if( ( f->getYRotation() > (3.14159f * 0.5f) - 0.1f &&
                  f->getYRotation() < (3.14159f * 0.5f) + 0.1f ) ||
                ( f->getYRotation() > (3.14159f * 1.5f) - 0.1f &&
                  f->getYRotation() < (3.14159f * 1.5f) + 0.1f ) ){
                float tmpL = left;
                float tmpR = right;
                left = front;
                right = back;
                front = tmpL;
                back = tmpR;
            }

            front += f->getPosition().z;
            back += f->getPosition().z;
            left += f->getPosition().x;
            right += f->getPosition().x;
                
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

    if( i >= 0 && i < mLevel.getWidth() &&
        j >= 0 && j < mLevel.getDepth() ){

        blockHeight = static_cast<float>(mLevel.getBlock(i, j).getHeight()) * 0.3f;

        if( blockHeight > pf ||
            ( fabs( (pf - 0.15f) - blockHeight ) < 0.05f &&
            mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Ramp &&
            ( mLevel.getBlock(i,j).getRamp() == Level::Ramp::Left ||
               mLevel.getBlock(i,j).getRamp() == Level::Ramp::Right ) )  ){
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

        //Check furniture
        if( Level::Furniture* f = mLevel.getBlock(i, j).getFurniture() ){

            float front = -mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
            float left = -mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
            float right = mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
            float back = mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;

            //If we have rotated 90 or 270 degrees, swap width and height
            if( ( f->getYRotation() > (3.14159f * 0.5f) - 0.1f &&
                  f->getYRotation() < (3.14159f * 0.5f) + 0.1f ) ||
                ( f->getYRotation() > (3.14159f * 1.5f) - 0.1f &&
                  f->getYRotation() < (3.14159f * 1.5f) + 0.1f ) ){
                float tmpL = left;
                float tmpR = right;
                left = front;
                right = back;
                front = tmpL;
                back = tmpR;
            }

            front += f->getPosition().z;
            back += f->getPosition().z;
            left += f->getPosition().x;
            right += f->getPosition().x;

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

    if( i >= 0 && i < mLevel.getWidth() &&
        j >= 0 && j < mLevel.getDepth() ){

        blockHeight = static_cast<float>(mLevel.getBlock(i, j).getHeight()) * 0.3f;

        if( blockHeight > pf ||
            ( fabs( (pf - 0.15f) - blockHeight ) < 0.05f &&
            mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Ramp &&
            ( mLevel.getBlock(i,j).getRamp() == Level::Ramp::Left ||
               mLevel.getBlock(i,j).getRamp() == Level::Ramp::Right ) ) ){
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

        //Check furniture
        if( Level::Furniture* f = mLevel.getBlock(i, j).getFurniture() ){

            float front = -mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
            float left = -mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
            float right = mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
            float back = mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;

            //If we have rotated 90 or 270 degrees, swap width and height
            if( ( f->getYRotation() > (3.14159f * 0.5f) - 0.1f &&
                  f->getYRotation() < (3.14159f * 0.5f) + 0.1f ) ||
                ( f->getYRotation() > (3.14159f * 1.5f) - 0.1f &&
                  f->getYRotation() < (3.14159f * 1.5f) + 0.1f ) ){
                float tmpL = left;
                float tmpR = right;
                left = front;
                right = back;
                front = tmpL;
                back = tmpR;
            }

            front += f->getPosition().z;
            back += f->getPosition().z;
            left += f->getPosition().x;
            right += f->getPosition().x;

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