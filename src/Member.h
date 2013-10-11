#ifndef PARTY_MEMBER_H
#define PARTY_MEMBER_H

#include "Utils.h"

class Member{
public:

    Member() : mExists(false), mIsReady(false), mTimeSinceSnapshot(-1.0f), mPrevSnapshotLenth( 1.0f ) {
        mName[0] = '\0';
        mCurPos.x = 0.0f; mCurPos.y = 0.0f; 
        mPrevPos.x = 0.0f; mPrevPos.y = 0.0f;
    }

    inline void setName( char* name );
    inline char* getName();

    inline void setExist( bool val );
    inline bool doesExist();

    inline void setReady( bool val );
    inline bool isReady();

    void updateSnapshot( XMFLOAT2& posSnapshot );

    void update( float dt );

    void interpPosition( XMFLOAT2& pos );

protected:

    float getTimeStepScale();

protected:

    bool mExists;
    bool mIsReady;

    char mName[ NAME_LEN ];

    XMFLOAT2 mCurPos;
    XMFLOAT2 mPrevPos;

    float mPrevSnapshotLenth;
    float mTimeSinceSnapshot;
};

inline void Member::setName( char* name ){strncpy(mName, name, NAME_LEN);}
inline char* Member::getName(){return mName;}

inline void Member::setExist( bool val ){mExists = val; if(!val){mTimeSinceSnapshot = -1.0f;}}
inline bool Member::doesExist(){return mExists;}

inline void Member::setReady( bool val ){mIsReady = val;}
inline bool Member::isReady(){return mIsReady;}

#endif