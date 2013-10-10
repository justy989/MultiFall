#ifndef CHARACTER_H
#define CHARACTER_H

#include "WorldEntity.h"

class Character : public WorldEntity{
public:

    enum Action{
        Idle,
        Melee,
        Cast,
        Use,
        Activate
    };

    Character();

    virtual void update( float dt );

    inline XMFLOAT4& getFacingDirection();

    inline void setID( ushort id );
    inline ushort getID();

    void walk();


protected:

    XMFLOAT4 mFacingDirection;

    ushort mID;

    Action mCurrentAction;

    //Stats

    //Inventory

    //AI 
};

inline XMFLOAT4& Character::getFacingDirection(){return mFacingDirection;}

inline void Character::setID( ushort id ){mID = id;}

inline ushort Character::getID(){return mID;}

#endif 