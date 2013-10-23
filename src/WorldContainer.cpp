#include "WorldContainer.h"

#include <assert.h>

WorldContainer::WorldContainer() :
    mCType(TNone),
    mCState(SNone),
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