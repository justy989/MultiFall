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

protected:

    std::vector< ItemDefinition > mDefinitions;

    char* mUsageStrings[ ITEM_USAGE_TYPE_COUNT ];
    char* mEquipStrings[ ITEM_EQUIPMENT_TYPE_COUNT ];
};

#endif