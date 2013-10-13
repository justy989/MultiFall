#ifndef ITEM_H
#define ITEM_H

#include "Utils.h"

#define ITEM_NAME_LEN 32
#define ITEM_MAX_TIERS 8
#define ITEM_MAX_MAGIC_PROPERTIES 8
#define ITEM_USAGE_TYPE_COUNT 5
#define ITEM_EQUIPMENT_TYPE_COUNT 9

struct ItemDefinition{
    enum Usage{
        UNone,
        Weapon,
        Armor,
        Ingredient,
        Quest
    };

    enum Equipment{
        ENone,
        OneHanded,
        TwoHanded,
        Head,
        Neck,
        Chest,
        Hands,
        Legs,
        Feet
    };

    struct Weight{
        ushort whole;
        ushort fraction;
    };

    struct WeaponDamage{
        ushort min;
        ushort max;
    };

    struct ArmorValue{
        uint armor;
    };

    ItemDefinition() :
        usage( Usage::UNone ),
        equipment( Equipment::ENone ),
        stackable(false),
        sellable(false),
        consumable(false),
        numTiers(0)
    {
        name[0] = '\0';
    }

    //Name of the item
    char name[ ITEM_NAME_LEN ];

    Usage usage; //How is the item used?
    Equipment equipment; //How is the item equipped? If eligible

    bool stackable; //Is the item able to be stacked
    bool consumable; //Is the item able to be used
    bool sellable; //Is the item able to be sold
    bool padding;

    struct TierInfo{
        union{
            WeaponDamage weaponDamage;
            ArmorValue armorValue;
        };

        char prefix[ ITEM_NAME_LEN ]; //Prefix
        uint value; //value of this tier
        Weight weight; //Weight of this tier

        //Magic properties
    };

    TierInfo tierInfo[ ITEM_MAX_TIERS ];
    uint numTiers;
};

struct Item{
    uint id; //Id to point to definition
    uint stack; //how many items are stacked if stackable
    uint tier; //tier representing which stats are used

    //Unique Magic properties
};

#endif