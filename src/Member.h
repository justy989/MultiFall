#ifndef PARTY_MEMBER_H
#define PARTY_MEMBER_H

#include "Utils.h"

class Member{
public:

    Member() : mExists(false), mIsReady(false) {mName[0] = '\0';}

    inline void setName( char* name );
    inline char* getName();

    inline void setExist( bool val );
    inline bool doesExist();

    inline void setReady( bool val );
    inline bool isReady();

protected:

    bool mExists;
    bool mIsReady;

    char mName[ NAME_LEN ];
};

inline void Member::setName( char* name ){strncpy(mName, name, NAME_LEN);}
inline char* Member::getName(){return mName;}

inline void Member::setExist( bool val ){mExists = val;}
inline bool Member::doesExist(){return mExists;}

inline void Member::setReady( bool val ){mIsReady = val;}
inline bool Member::isReady(){return mIsReady;}

#endif