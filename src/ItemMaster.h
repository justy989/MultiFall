#ifndef ITEM_MASTER_H
#define ITEM_MASTER_H

#include "Item.h"

#include <vector>

class ItemMaster{
public:

    ItemMaster();

    bool init( char* masterFilePath );
    void clear();

    ItemDefinition* getDefinitionFromID( uint id );

    inline uint getNumIDs();

protected:

    std::vector< ItemDefinition > mDefinitions;
};

inline uint ItemMaster::getNumIDs(){return mDefinitions.size();}

#endif