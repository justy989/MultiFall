#include "Member.h"

void Member::updateSnapshot( XMFLOAT2& posSnapshot )
{
    mPrevPos = mCurPos;
    mCurPos = posSnapshot;

    //If this is the first time we are setting the snapshot
    //We need them to be equal
    if( mTimeSinceSnapshot < 0.0f ){
        mPrevPos = posSnapshot;
    }else{//Otherwise save the previous snapshot
        mPrevSnapshotLenth = mTimeSinceSnapshot;
        mPrevSnapshotLenth += 0.01f; //Make sure we can never divide by 0, muhahah
    }

    mTimeSinceSnapshot = 0.0f;
}

void Member::update( float dt )
{
    mTimeSinceSnapshot += dt;
}

float Member::getTimeStepScale()
{
    return mTimeSinceSnapshot / mPrevSnapshotLenth;
}

void Member::interpPosition( XMFLOAT2& pos )
{
    float timeStep = getTimeStepScale() * 0.9f;

    pos.x = ( ( mCurPos.x - mPrevPos.x ) * timeStep ) + mCurPos.x;
    pos.y = ( ( mCurPos.y - mPrevPos.y ) * timeStep ) + mCurPos.y;
}