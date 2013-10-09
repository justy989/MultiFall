#ifndef PARTY_MEMBER_H
#define PARTY_MEMBER_H

#define MEMBER_NAME_LEN 32

class Member{
public:

    Member() : mExists(false) {mName[0] = '\0';}

    inline void setName( char* name );
    inline char* getName();

    inline void setExist( bool val );
    inline bool doesExist();

protected:

    bool mExists;

    char mName[ MEMBER_NAME_LEN ];
};

inline void Member::setName( char* name ){strncpy(mName, name, MEMBER_NAME_LEN);}
inline char* Member::getName(){return mName;}

inline void Member::setExist( bool val ){mExists = val;}
inline bool Member::doesExist(){return mExists;}

#endif