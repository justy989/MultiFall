#include "ItemMaster.h"
#include "Log.h"

#include <fstream>
#include <assert.h>

ItemMaster::ItemMaster()
{
    mUsageStrings[ 0 ] = "None";
    mUsageStrings[ 1 ] = "Weapon";
    mUsageStrings[ 2 ] = "Armor";
    mUsageStrings[ 3 ] = "Ingredient";
    mUsageStrings[ 4 ] = "Quest";

    mEquipStrings[ 0 ] = "None";
    mEquipStrings[ 1 ] = "OneHanded";
    mEquipStrings[ 2 ] = "TwoHanded";
    mEquipStrings[ 3 ] = "Head";
    mEquipStrings[ 5 ] = "Neck";
    mEquipStrings[ 6 ] = "Chest";
    mEquipStrings[ 7 ] = "Hands";
    mEquipStrings[ 8 ] = "Legs";
    mEquipStrings[ 9 ] = "Feet";
}

bool ItemMaster::init( char* masterFilePath )
{
    //
    std::ifstream checkFile( masterFilePath );
    char buffer[ 128 ];
    uint id;
    uint tier;
    ItemDefinition definition;

    if( !checkFile.is_open() ){
        LOG_ERRO << "Unable to open item master file: " << masterFilePath << LOG_ENDL;
        return false;
    }

    while( !checkFile.eof() ){
        checkFile.getline( buffer, 128 );

        //Allow Comments
        for(int i = 0; i < strlen(buffer); i++){
            if( buffer[i] == '#'){
                buffer[i] = '\0';
                break;
            }
        }

        //Blank lines should be skipped
        if( strlen( buffer ) == 0 ){
            continue;
        }

        char* setting = strtok(buffer, " ");
        char* equals = strtok(NULL, " ");
        char* value = strtok(NULL, " ");

        if( strcmp( setting, "Name" ) == 0 ){
            if( id > 0 ){
                mDefinitions.push_back( definition );
            }

            definition = ItemDefinition();

            strncpy(definition.name, value, ITEM_NAME_LEN );
            id++;
            continue;
        }else if( strcmp( setting, "Usage" ) == 0 ){
            for(int i = 0; i < ITEM_USAGE_TYPE_COUNT; i++){
                if( strcmp( value, mUsageStrings[i] ) == 0 ){
                    definition.usage = (ItemDefinition::Usage)(i);
                }
            }
            continue;
        }else if( strcmp( setting, "Equipment" ) == 0 ){
            for(int i = 0; i < ITEM_EQUIPMENT_TYPE_COUNT; i++){
                if( strcmp( value, mEquipStrings[i] ) == 0 ){
                    definition.equipment = (ItemDefinition::Equipment)(i);
                }
            }
            continue;
        }else if( strcmp( setting, "Stackable" ) == 0 ){
            definition.stackable = static_cast<bool>( atoi( value ) );
            continue;
        }else if( strcmp( setting, "Consumable" ) == 0 ){
            definition.consumable = static_cast<bool>( atoi( value ) );
            continue;
        }else if( strcmp( setting, "Sellable" ) == 0 ){
            definition.sellable = static_cast<bool>( atoi( value ) );
            continue;
        }else if( strcmp( setting, "TierCount" ) == 0 ){
            definition.numTiers = atoi( value );
            assert( definition.numTiers < ITEM_MAX_TIERS );
            tier = 0;
            continue;
        }else if( strcmp( setting, "TierPrefix" ) == 0 ){
            strncpy( definition.tierInfo[ tier ].prefix, value, ITEM_NAME_LEN );
            continue;
        }else if( strcmp( setting, "TierMinDamage" ) == 0 ){
            definition.tierInfo[ tier ].weaponDamage.min = atoi( value );
            continue;
        }else if( strcmp( setting, "TierMaxDamage" ) == 0 ){
            definition.tierInfo[ tier ].weaponDamage.max = atoi( value );
            tier++;
            assert( tier <= definition.numTiers );
            continue;
        }else if( strcmp( setting, "TierValue" ) == 0 ){
            definition.tierInfo[ tier ].value = atoi( value );
            continue;
        }else if( strcmp( setting, "TierWeightWhole" ) == 0 ){
            definition.tierInfo[ tier ].weight.whole = atoi( value );
            continue;
        }else if( strcmp( setting, "TierWeightFraction" ) == 0 ){
            definition.tierInfo[ tier ].weight.fraction = atoi( value );
            continue;
        }else if( strcmp( setting, "TierArmorValue" ) == 0 ){
            definition.tierInfo[ tier ].armorValue.armor = atoi( value );
            tier++;
            assert( tier <= definition.numTiers );
            continue;
        }
    }

    mDefinitions.push_back( definition );

    checkFile.close();

    return true;
}

void ItemMaster::clear()
{
    mDefinitions.clear();
}

ItemDefinition* ItemMaster::getDefinitionFromID( uint id )
{
    if( id < mDefinitions.size() ){
        return &mDefinitions[ id ];
    }

    return NULL;
}