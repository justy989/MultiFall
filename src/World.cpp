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

    for(ushort i = 0; i < mLevel.getNumDoors(); i++){
        Level::Door& door = mLevel.getDoor(i);

        if( door.state == Level::Door::Opening ){
            door.setYRotation( door.getYRotation() + 0.02f );

            if( door.getYRotation() > ( door.startAngle + ( 3.14159f * 0.5f ) ) ){
                door.state = Level::Door::Opened;
                door.setYRotation( door.startAngle + ( 3.14159f * 0.5f ) );
            }
        }else if( door.state == Level::Door::Closing ){
            door.setYRotation( door.getYRotation() - 0.02f );

            if( door.getYRotation() < ( door.startAngle - ( 3.14159f * 0.5f ) ) ){
                door.state = Level::Door::Closed;
                door.setYRotation( door.startAngle - ( 3.14159f * 0.5f ) );
            }
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
                ty = 0.25f;
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
    case Event::Type::DoorOpen:
        mLevel.getDoor( e.doorOpenInfo.id ).open();
        break;
    case Event::Type::DoorClose:
        mLevel.getDoor( e.doorCloseInfo.id ).close();
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

    XMVECTOR wallX = XMVectorZero();
    XMVECTOR wallZ = XMVectorZero();

    float bLeft;
    float bRight;
    float bFront;
    float bBack;

    bool collided = false;

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

    Level::Door* centerDoor = NULL;

    //Check for doors in the current block
    if( mLevel.getBlock(bX,bZ).getCollidableType() == Level::Block::Collidable::Door ){
        centerDoor = mLevel.getBlock(bX, bZ).getDoor();
        
        if( centerDoor->state == Level::Door::Closed ){
            bFront = static_cast<float>(bZ) * 0.3f;
            bLeft = static_cast<float>(bX) * 0.3f;
            bRight = bLeft + 0.3f;
            bBack = bFront + 0.3f;

            switch( centerDoor->facing ){
            case 0:
                if( WorldEntity::circleAALineIntersect(
                                XMVectorSet(bLeft, 0.0f, bFront, 1.0f),
                                XMVectorSet(bRight, 0.0f, bFront, 1.0f),
                                XMVectorSet(px, 0.0f, pz, 1.0f),
                                entity->getSolidity().radius ) ){
                    wallZ = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f );
                    collided = true;
                }
                break;
            case 1:
                if( WorldEntity::circleAALineIntersect(
                                XMVectorSet(bLeft, 0.0f, bFront, 1.0f),
                                XMVectorSet(bLeft, 0.0f, bBack, 1.0f),
                                XMVectorSet(px, 0.0f, pz, 1.0f),
                                entity->getSolidity().radius ) ){
                    wallX = XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f );
                    collided = true;
                }
                break;
            case 2:
                if( WorldEntity::circleAALineIntersect(
                                XMVectorSet(bLeft, 0.0f, bBack, 1.0f),
                                XMVectorSet(bRight, 0.0f, bBack, 1.0f),
                                XMVectorSet(px, 0.0f, pz, 1.0f),
                                entity->getSolidity().radius ) ){
                    wallZ = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f );
                    collided = true;
                }
                break;
            case 3:
                if( WorldEntity::circleAALineIntersect(
                                XMVectorSet(bRight, 0.0f, bFront, 1.0f),
                                XMVectorSet(bRight, 0.0f, bBack, 1.0f),
                                XMVectorSet(px, 0.0f, pz, 1.0f),
                                entity->getSolidity().radius ) ){
                    wallX = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f );
                    collided = true;
                }
                break;
            default:
                break;
            }
        }
    }

    //Check the left Block's right side
    i = bX - 1;
    j = bZ;

    if( i >= 0 && i < mLevel.getWidth() &&
        j >= 0 && j < mLevel.getDepth() ){

        //Check wall
        if( mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Wall ||
            ( mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Door &&
              mLevel.getBlock(i,j).getDoor()->state == Level::Door::Closed &&
              mLevel.getBlock(i,j).getDoor()->facing == 3 ) ){
            bLeft = static_cast<float>(i) * 0.3f;
            bFront = static_cast<float>(j) * 0.3f;
            bRight = bLeft + 0.3f;
            bBack = bFront + 0.3f;

            if( WorldEntity::circleAALineIntersect(
                            XMVectorSet(bRight, 0.0f, bFront, 1.0f),
                            XMVectorSet(bRight, 0.0f, bBack, 1.0f),
                            XMVectorSet(px, 0.0f, pz, 1.0f),
                            entity->getSolidity().radius ) ){
                wallX = XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f );
                collided = true;
            }
        }
        
        //Check to see if solid objects are there
        Level::SolidObject* so = NULL;

        switch( mLevel.getBlock(i, j).getCollidableType() ){
        case Level::Block::Collidable::Furniture:
            if( Level::Furniture* f = mLevel.getBlock(i, j).getFurniture() ){
                bFront = -mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
                bLeft = -mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
                bRight = mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
                bBack = mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
                so = f;
            }
            break;
        case Level::Block::Collidable::Container:
            if( Level::Container* f = mLevel.getBlock(i, j).getContainer() ){
                bFront = -mLevel.getContainerDimensions( f->getCType() ).z / 2.0f;
                bLeft = -mLevel.getContainerDimensions( f->getCType() ).x / 2.0f;
                bRight = mLevel.getContainerDimensions( f->getCType() ).x / 2.0f;
                bBack = mLevel.getContainerDimensions( f->getCType() ).z / 2.0f;
                so = f;
            }
            break;
        default:
            break;
        }

        if( so ){
            //If we have rotated 90 or 270 degrees, swap width and height
            if( ( so->getYRotation() > (3.14159f * 0.5f) - 0.1f &&
                    so->getYRotation() < (3.14159f * 0.5f) + 0.1f ) ||
                ( so->getYRotation() > (3.14159f * 1.5f) - 0.1f &&
                    so->getYRotation() < (3.14159f * 1.5f) + 0.1f ) ){
                float tmpL = bLeft;
                float tmpR = bRight;
                bLeft = bFront;
                bRight = bBack;
                bFront = tmpL;
                bBack = tmpR;
            }

            bFront += so->getPosition().z;
            bBack += so->getPosition().z;
            bLeft += so->getPosition().x;
            bRight += so->getPosition().x;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(bRight, 0.0f, bFront, 1.0f),
                            XMVectorSet(bRight, 0.0f, bBack, 1.0f),
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

        if( mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Wall ||
            ( mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Door &&
              mLevel.getBlock(i,j).getDoor()->state == Level::Door::Closed &&
              mLevel.getBlock(i,j).getDoor()->facing == 1 ) ){
            bLeft = static_cast<float>(i) * 0.3f;
            bFront = static_cast<float>(j) * 0.3f;
            bRight = bLeft + 0.3f;
            bBack = bFront + 0.3f;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(bLeft, 0.0f, bFront, 1.0f),
                            XMVectorSet(bLeft, 0.0f, bBack, 1.0f),
                            XMVectorSet(px, 0.0f, pz, 1.0f),
                            entity->getSolidity().radius ) ){
                wallX = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f );
                collided = true;
            }
        }

        //Check to see if solid objects are there
        Level::SolidObject* so = NULL;

        switch( mLevel.getBlock(i, j).getCollidableType() ){
        case Level::Block::Collidable::Furniture:
            if( Level::Furniture* f = mLevel.getBlock(i, j).getFurniture() ){
                bFront = -mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
                bLeft = -mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
                bRight = mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
                bBack = mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
                so = f;
            }
            break;
        case Level::Block::Collidable::Container:
            if( Level::Container* f = mLevel.getBlock(i, j).getContainer() ){
                bFront = -mLevel.getContainerDimensions( f->getCType() ).z / 2.0f;
                bLeft = -mLevel.getContainerDimensions( f->getCType() ).x / 2.0f;
                bRight = mLevel.getContainerDimensions( f->getCType() ).x / 2.0f;
                bBack = mLevel.getContainerDimensions( f->getCType() ).z / 2.0f;
                so = f;
            }
            break;
        default:
            break;
        }

        if( so ){
            if( ( so->getYRotation() > (3.14159f * 0.5f) - 0.1f &&
                    so->getYRotation() < (3.14159f * 0.5f) + 0.1f ) ||
                ( so->getYRotation() > (3.14159f * 1.5f) - 0.1f &&
                    so->getYRotation() < (3.14159f * 1.5f) + 0.1f ) ){
                float tmpL = bLeft;
                float tmpR = bRight;
                bLeft = bFront;
                bRight = bBack;
                bFront = tmpL;
                bBack = tmpR;
            }

            bFront += so->getPosition().z;
            bBack += so->getPosition().z;
            bLeft += so->getPosition().x;
            bRight += so->getPosition().x;
                
            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(bLeft, 0.0f, bFront, 1.0f),
                            XMVectorSet(bLeft, 0.0f, bBack, 1.0f),
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

        if( mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Wall ||
            ( mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Door &&
              mLevel.getBlock(i,j).getDoor()->state == Level::Door::Closed &&
              mLevel.getBlock(i,j).getDoor()->facing == 2 ) ){
            bLeft = static_cast<float>(i) * 0.3f;
            bFront = static_cast<float>(j) * 0.3f;
            bRight = bLeft + 0.3f;
            bBack = bFront + 0.3f;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(bLeft, 0.0f, bBack, 1.0f),
                            XMVectorSet(bRight, 0.0f, bBack, 1.0f),
                            XMVectorSet(px, 0.0f, pz, 1.0f),
                            entity->getSolidity().radius ) ){
                wallZ = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f );
                collided = true;
            }
        }

        //Check to see if solid objects are there
        Level::SolidObject* so = NULL;

        switch( mLevel.getBlock(i, j).getCollidableType() ){
        case Level::Block::Collidable::Furniture:
            if( Level::Furniture* f = mLevel.getBlock(i, j).getFurniture() ){
                bFront = -mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
                bLeft = -mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
                bRight = mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
                bBack = mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
                so = f;
            }
            break;
        case Level::Block::Collidable::Container:
            if( Level::Container* f = mLevel.getBlock(i, j).getContainer() ){
                bFront = -mLevel.getContainerDimensions( f->getCType() ).z / 2.0f;
                bLeft = -mLevel.getContainerDimensions( f->getCType() ).x / 2.0f;
                bRight = mLevel.getContainerDimensions( f->getCType() ).x / 2.0f;
                bBack = mLevel.getContainerDimensions( f->getCType() ).z / 2.0f;
                so = f;
            }
            break;
        default:
            break;
        }

        if( so ){
            //If we have rotated 90 or 270 degrees, swap width and height
            if( ( so->getYRotation() > (3.14159f * 0.5f) - 0.1f &&
                    so->getYRotation() < (3.14159f * 0.5f) + 0.1f ) ||
                ( so->getYRotation() > (3.14159f * 1.5f) - 0.1f &&
                    so->getYRotation() < (3.14159f * 1.5f) + 0.1f ) ){
                float tmpL = bLeft;
                float tmpR = bRight;
                bLeft = bFront;
                bRight = bBack;
                bFront = tmpL;
                bBack = tmpR;
            }

            bFront += so->getPosition().z;
            bBack += so->getPosition().z;
            bLeft += so->getPosition().x;
            bRight += so->getPosition().x;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(bLeft, 0.0f, bBack, 1.0f),
                            XMVectorSet(bRight, 0.0f, bBack, 1.0f),
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

        if( mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Wall ||
            ( mLevel.getBlock(i,j).getCollidableType() == Level::Block::Collidable::Door &&
              mLevel.getBlock(i,j).getDoor()->state == Level::Door::Closed &&
              mLevel.getBlock(i,j).getDoor()->facing == 0 ) ){
            bLeft = static_cast<float>(i) * 0.3f;
            bFront = static_cast<float>(j) * 0.3f;
            bRight = bLeft + 0.3f;
            bBack = bFront + 0.3f;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(bLeft, 0.0f, bFront, 1.0f),
                            XMVectorSet(bRight, 0.0f, bFront, 1.0f),
                            XMVectorSet(px, 0.0f, pz, 1.0f),
                            entity->getSolidity().radius ) ){
                wallZ = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f );
                collided = true;
            }
        }

        //Check to see if solid objects are there
        Level::SolidObject* so = NULL;

        switch( mLevel.getBlock(i, j).getCollidableType() ){
        case Level::Block::Collidable::Furniture:
            if( Level::Furniture* f = mLevel.getBlock(i, j).getFurniture() ){
                bFront = -mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
                bLeft = -mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
                bRight = mLevel.getFurnitureDimensions( f->getType() ).x / 2.0f;
                bBack = mLevel.getFurnitureDimensions( f->getType() ).z / 2.0f;
                so = f;
            }
            break;
        case Level::Block::Collidable::Container:
            if( Level::Container* f = mLevel.getBlock(i, j).getContainer() ){
                bFront = -mLevel.getContainerDimensions( f->getCType() ).z / 2.0f;
                bLeft = -mLevel.getContainerDimensions( f->getCType() ).x / 2.0f;
                bRight = mLevel.getContainerDimensions( f->getCType() ).x / 2.0f;
                bBack = mLevel.getContainerDimensions( f->getCType() ).z / 2.0f;
                so = f;
            }
            break;
        default:
            break;
        }

        if( so ){
            //If we have rotated 90 or 270 degrees, swap width and height
            if( ( so->getYRotation() > (3.14159f * 0.5f) - 0.1f &&
                    so->getYRotation() < (3.14159f * 0.5f) + 0.1f ) ||
                ( so->getYRotation() > (3.14159f * 1.5f) - 0.1f &&
                    so->getYRotation() < (3.14159f * 1.5f) + 0.1f ) ){
                float tmpL = bLeft;
                float tmpR = bRight;
                bLeft = bFront;
                bRight = bBack;
                bFront = tmpL;
                bBack = tmpR;
            }

            bFront += so->getPosition().z;
            bBack += so->getPosition().z;
            bLeft += so->getPosition().x;
            bRight += so->getPosition().x;

            if( WorldEntity::circleAALineIntersect( 
                            XMVectorSet(bLeft, 0.0f, bFront, 1.0f),
                            XMVectorSet(bRight, 0.0f, bFront, 1.0f),
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