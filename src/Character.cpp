#include "Character.h"

Character::Character() : mAlliance( Party )
{
    mFacingDirection.x = 1.0f;
    mFacingDirection.y = 0.0f;
    mFacingDirection.z = 0.0f;
    mFacingDirection.w = 1.0f;
}

void Character::update( float dt )
{
    switch( mCurrentAction.type ){
    case Character::Action::TIdle:
        mCurrentAction.progress += dt;
        break;
    case Character::Action::Melee:
        break;
    case Character::Action::Ranged:
        break;
    case Character::Action::Cast:
        break;
    }

    mCurrentAction.state = (Character::Action::State)( (int)( mCurrentAction.progress / 0.25f ) );

    if( mCurrentAction.state > Character::Action::CoolDown ){
        mCurrentAction.state = Character::Action::SIdle;
    }

    if( mCurrentAction.progress > 1.0f ){
        mCurrentAction.progress = 0.0f;
    }

    mMoveProgress += dt;
}

bool Character::move( Character::MoveState state )
{
    mCurrentMoveState = state;
    mMoveProgress = 0.0f;

    return true;
}

bool Character::action( Character::Action::Type type )
{
    if( mCurrentAction.state != Character::Action::SIdle ){
        return false;
    }

    mCurrentAction.type = type;
    mCurrentAction.state = Character::Action::WarmUp;
    mCurrentAction.progress = 0.0f;

    return true;
}