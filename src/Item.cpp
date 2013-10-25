#include "Item.h"

#include <iostream>

char* ItemDefinition::usageStrings[ 5 ] = 
{
    "None",
    "Weapon",
    "Armor",
    "Consume",
    "Quest"
};

char* ItemDefinition::armorTypeStrings[ 7 ]= 
{
    "None",
    "Head",
    "Neck",
    "Chest",
    "Hands",
    "Legs",
    "Feet"
};

char* ItemDefinition::weaponTypeStrings[ 4 ]= 
{
    "None",
    "One-Handed",
    "Two-Handed",
    "Ranged"
};

char* ItemDefinition::consumableTypeStrings[ 3 ]= 
{
        "None",
        "Ingred",
        "Potion"
};

void ItemDefinition::buildStringFromItem(char* buffer, ItemDefinition* item, uint stack)
{
    //Name
    strcpy( buffer, item->name );

    //Type
    switch( item->usage ){
    case ItemDefinition::Usage::Armor:
        sprintf(buffer, "%s x%d\n%s:%s\n%d Armor\n",
            buffer, stack,
            usageStrings[ Armor ], 
            armorTypeStrings[ item->armorType ],
            item->armorValue );
        break;
    case ItemDefinition::Usage::Weapon:
        sprintf(buffer, "%s x%d\n%s:%s\n%d-%d Damage\n", 
            buffer, stack,
            usageStrings[ Weapon ], 
            weaponTypeStrings[ item->weaponType ],
            item->weaponDamage.min,
            item->weaponDamage.max);
        break;
    case ItemDefinition::Usage::Consumable:
        sprintf(buffer, "%s x%d\n%s\n%s\n", buffer, stack,
            usageStrings[ Consumable ], consumableTypeStrings[ item->consumableType ] );
        break;
    case ItemDefinition::Usage::Quest:
        break;
    default:
        break;
    }

    //Weight Value
    sprintf(buffer, "%s%dg %dlbs\n", buffer, item->value, item->weight.whole );
}