#ifndef CHARACTER_H
#define CHARACTER_H

#include "WorldEntity.h"

#define CHARACTER_MOVE_SPEED 1.25f

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
    inline XMFLOAT4& getWalkingDirection();

    inline void setID( ushort id );
    inline ushort getID();

    void walk( XMFLOAT4& direction );
    void stop();


protected:

    XMFLOAT4 mFacingDirection;
    XMFLOAT4 mWalkingDirection;

    ushort mID;

    Action mCurrentAction;

    //Stats

    //Inventory

    //AI 
};

inline XMFLOAT4& Character::getFacingDirection(){return mFacingDirection;}
inline XMFLOAT4& Character::getWalkingDirection(){return mWalkingDirection;}

inline void Character::setID( ushort id ){mID = id;}

inline ushort Character::getID(){return mID;}

#endif 