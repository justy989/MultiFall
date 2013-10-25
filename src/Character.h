#ifndef CHARACTER_H
#define CHARACTER_H

#include "WorldEntity.h"
#include "WorldContainer.h"

#define CHARACTER_MOVE_SPEED 1.25f

#define CHAR_AI_FLAG_USES_RANGED 1
#define CHAR_AI_FLAG_USES_SPELLS 2
#define CHAR_AI_FLAG_HEALS_OTHERS 4
#define CHAR_AI_FLAG_RUN_AWAY_ON_LOW_HEALTH 8
#define CHAR_AI_FLAG_SPRINT_TO_TARGET 16

class Character : public WorldEntity, public WorldContainer{
public:

    //Action
    struct Action{
        enum Type{
            TIdle,
            Activate,
            Melee,
            Ranged,
            Cast,
            Consume
        };

        enum State{
            SIdle,
            WarmUp,
            Climax,
            CoolDown
        };

        Action() :
            type(TIdle),
            state(SIdle),
            progress(0.0f) {}

        Type type;
        State state;
        float progress;
    };

    //Move state
    enum MoveState{
        Idle,
        Crouch,
        Walk,
        Run
    };

    enum Alliance{
        Party,
        Enemy
    };

    Character();

    virtual void update( float dt );

    bool move( MoveState state );
    bool action( Action::Type type );

    inline XMFLOAT4& getFacingDirection();
    inline XMFLOAT4& getWalkingDirection();

    inline void setAlliance( Alliance alliance );

    inline void setID( ushort id );
    inline ushort getID();

    inline Action::Type getCurrentActionType();
    inline Action::State getCurrentActionState();
    inline float getActionProgress();

    float getDistanceToCharacter( Character& c );

protected:

    XMFLOAT4 mFacingDirection;
    XMFLOAT4 mWalkingDirection;

    ushort mID;

    Action mCurrentAction;

    MoveState mCurrentMoveState;
    float mMoveProgress;

    //Stats
    Stats mStats;

    //AI
    uint mAIFlags;

    Alliance mAlliance;
};

inline XMFLOAT4& Character::getFacingDirection(){return mFacingDirection;}
inline XMFLOAT4& Character::getWalkingDirection(){return mWalkingDirection;}

inline void Character::setID( ushort id ){mID = id;}

inline ushort Character::getID(){return mID;}

inline void Character::setAlliance( Alliance alliance )
{
    mAlliance = alliance;
}

inline Character::Action::Type Character::getCurrentActionType(){return mCurrentAction.type;}
inline Character::Action::State Character::getCurrentActionState(){return mCurrentAction.state;}
inline float Character::getActionProgress(){return mCurrentAction.progress;}

#endif 