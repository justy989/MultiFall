#include "ItemMaster.h"
#include "Log.h"

#include <fstream>
#include <assert.h>

ItemMaster::ItemMaster()
{

}

bool ItemMaster::init( char* masterFilePath )
{
    //
    std::ifstream checkFile( masterFilePath );
    char buffer[ 128 ];
    ItemDefinition definition;

    if( !checkFile.is_open() ){
        LOG_ERRO << "Unable to open item master file: " << masterFilePath << LOG_ENDL;
        return false;
    }

    LOG_INFO << "Loading Item Master List" << LOG_ENDL;

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

        int len = strlen( buffer );

        if( buffer[0] != '[' && buffer[ len - 1 ] != ']' ){
            LOG_ERRO << "Expected new item instead of line: " << buffer << LOG_ENDL;
            return false;
        }

        if( strlen( definition.name ) ){
            mDefinitions.push_back( definition );
        }

        int endChar = len - 2 < ITEM_NAME_LEN ? len - 2: ITEM_NAME_LEN;

        //Copy the item name
        strncpy( definition.name, buffer + 1, endChar ); 

        definition.name[ endChar ] = '\0';

        //Read Type
        checkFile.getline( buffer, 128 );

        switch( buffer[0] ){
        case 'W':
            {
                definition.usage = ItemDefinition::Usage::Weapon;

                if( buffer[1] == '1' ){
                    definition.weaponType = ItemDefinition::WeaponType::OneHanded;
                }else if( buffer[1] == '2' ){
                    definition.weaponType = ItemDefinition::WeaponType::TwoHanded;
                }else if( buffer[1] == 'R' ){
                    definition.weaponType = ItemDefinition::WeaponType::Ranged;
                }else{
                    LOG_ERRO << "Invalid weapon type: " << buffer[2] << LOG_ENDL;
                    return false;
                }

                //Read damage
                checkFile.getline( buffer, 128 );

                int rc = sscanf( buffer, "%d-%d", &definition.weaponDamage.min, &definition.weaponDamage.max );

                if( rc != 2 ){
                    LOG_ERRO << "Didn't see expected line for weapon damage formatted: Min-Max" << LOG_ENDL;
                    return false;
                }
            }
            break;
        case 'A':
            {
                definition.usage = ItemDefinition::Usage::Armor;

                if( buffer[1] == 'C' ){
                    definition.armorType = ItemDefinition::ArmorType::Chest;
                }else if( buffer[1] == 'F' ){
                    definition.armorType = ItemDefinition::ArmorType::Feet;
                }else if( buffer[1] == 'H' ){
                    definition.armorType = ItemDefinition::ArmorType::Head;
                }else if( buffer[1] == 'A' ){
                    definition.armorType = ItemDefinition::ArmorType::Hands;
                }else if( buffer[1] == 'L' ){
                    definition.armorType = ItemDefinition::ArmorType::Legs;
                }else if( buffer[1] == 'N' ){
                    definition.armorType = ItemDefinition::ArmorType::Neck;
                }else{
                    LOG_ERRO << "Invalid armor type: " << buffer[2] << LOG_ENDL;
                    return false;
                }

                //Read damage
                checkFile.getline( buffer, 128 );

                int rc = sscanf( buffer, "%d", &definition.armorValue.armor );

                if( rc != 1 ){
                    LOG_ERRO << "Didn't see expected line for Armor Value formatted: Value" << LOG_ENDL;
                    return false;
                }
            }
            break;
        case 'C':
            {
                definition.usage = ItemDefinition::Usage::Consumable;

                if( buffer[1] == 'I' ){
                    definition.consumableType = ItemDefinition::ConsumableType::Ingredient;
                }else if( buffer[1] == 'P' ){
                    definition.consumableType = ItemDefinition::ConsumableType::Potion;
                }else{
                    LOG_ERRO << "Invalid consumable type: " << buffer[2] << LOG_ENDL;
                    return false;
                }
            }
            break;
        case 'Q':
            break;
        default:
            break;
        }

        //Read value and weight
        checkFile.getline( buffer, 128 );

        int rc = sscanf( buffer, "%dg %dlbs", &definition.value, &definition.weight.whole );

        if( rc != 2 ){
            LOG_ERRO << "Didn't see expected line for value and weight formatted: Ng Nlbs" << LOG_ENDL;
            return false;
        }

        //Get the flags
        checkFile.getline( buffer, 128 );

        for(uint i = 0; i < strlen(buffer); i++){
            switch( buffer[i] ){
            case 'T':
                definition.sellable = true;
                break;
            case 'S':
                definition.stackable = true;
                break;
            default:
                break;
            }
        }
    }

    if( strlen( definition.name ) ){
        mDefinitions.push_back( definition );
    }

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