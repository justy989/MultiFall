#ifndef ITEM_H
#define ITEM_H

#include "Utils.h"
#include "Stats.h"

#define ITEM_NAME_LEN 32
#define ITEM_MAX_MAGIC_PROPERTIES 8
#define ITEM_USAGE_TYPE_COUNT 5
#define ITEM_EQUIPMENT_TYPE_COUNT 9
#define ITEM_MAX_STAT_EFFECTS 8

struct ItemDefinition{
    enum Usage{
        UNone,
        Weapon,
        Armor,
        Consumable,
        Quest
    };

    enum WeaponType{
        WNone,
        OneHanded,
        TwoHanded,
        Ranged
    };

    enum ArmorType{
        ANone,
        Head,
        Neck,
        Chest,
        Hands,
        Legs,
        Feet
    };

    enum ConsumableType{
        CNone,
        Ingredient,
        Potion
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
        stackable(false),
        sellable(false)
    {
        name[0] = '\0';
    }

    //Name of the item
    char name[ ITEM_NAME_LEN ];

    Usage usage; //How is the item used?

    union{
        WeaponType weaponType;
        ArmorType armorType;
        ConsumableType consumableType;
    };

    bool stackable; //Is the item able to be stacked
    bool sellable; //Is the item able to be sold

    union{
        WeaponDamage weaponDamage; //Damage a weapon does
        ArmorValue armorValue; //Armor value
    };

    uint value; //value of this tier
    Weight weight; //Weight of this tier
};

class Item{
public:
    uint id; //Id to point to definition
    uint stack; //how many items are stacked if stackable

    //Unique Magic properties
    Stats::Effect mEffects[ ITEM_MAX_STAT_EFFECTS ];

    bool operator==(Item item){return id == item.id && stack == item.stack;}
};

#endif