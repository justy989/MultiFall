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