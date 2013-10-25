#include "WorldContainer.h"

#include <assert.h>

WorldContainer::WorldContainer() :
    mCType(TNone),
    mCState(Closed),
    mGold(0)
{

}

uint WorldContainer::getNumItems()
{
    return mItems.size();
}

Item* WorldContainer::getItem( uint index )
{
    assert( index < mItems.size() );

    //Advance to the index
    std::list<Item>::iterator it = mItems.begin();
    std::advance(it, index);

    return & ( *it );
}

void WorldContainer::addItem( Item& item )
{
    //Iterate through the list

    //If IDs match, add to the stack
    for (std::list<Item>::iterator it = mItems.begin(); it != mItems.end(); ++it){
        if( (*it).id == item.id ){
            (*it).stack++;
            return;
        }
    }

    item.stack = 1;
    mItems.push_back( item );
}

void WorldContainer::takeItem( uint index )
{
    assert( index < mItems.size() );

    //Advance to the index
    std::list<Item>::iterator it = mItems.begin();
    std::advance(it, index);

    //Remove it
    mItems.remove( *it );
}

void WorldContainer::addGold( uint gold )
{
    mGold += gold;
}

bool WorldContainer::takeGold( uint gold )
{
    if( gold < mGold ){
        mGold -= gold;
        return true;
    }

    return false;
}

bool WorldContainer::open()
{
    if( getNumItems() &&
        mCState == WorldContainer::State::Closed ){
        mCState = WorldContainer::State::Open;
        return true;
    }

    return false;
}

void WorldContainer::close()
{
    mCState = WorldContainer::State::Closed;
}