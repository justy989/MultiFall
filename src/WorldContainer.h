#ifndef WORLD_CONTAINER_H
#define WORLD_CONTAINER_H

#include "Item.h"

#include <list>

#define WORLD_CONTAINER_TYPE_COUNT 6

typedef std::list< Item > Inventory;

class WorldContainer{
public:

    enum Type{
        TNone,
        Crate,
        Barrel,
        Chest,
        Dresser,
        Character,
    };

    enum State{
        SNone,
        Closed,
        Opening,
        Open,
        Occupied
    };

    WorldContainer();

    inline void setCType( Type type );
    inline Type getCType();

    inline void setCState( State State );
    inline State getCState();

    //Leaving these undefined for now
    uint getNumItems(){}
    Item* getItem( uint index ){}

    void addItem( Item& item ){}
    Item* takeItem( uint index ){}

    void addGold( uint gold ){}
    bool takeGold( uint gold ){}

protected:

    Type mCType;
    State mCState;

    //List of items
    Inventory mItems;

    uint mGold;
};

inline void WorldContainer::setCType( WorldContainer::Type type ){mCType = type;}
inline WorldContainer::Type WorldContainer::getCType(){return mCType;}

inline void WorldContainer::setCState( WorldContainer::State State ){mCState = State;}
inline WorldContainer::State WorldContainer::getCState(){return mCState;}

#endif