#ifndef STATS_H
#define STATS_H

#include "Utils.h"

#define STATS_MAX_ACTIVE_EFFECTS 16

class Stats{
public:

    struct Effect{

        //Which stat are we effecting
        enum StatID{
            None,

            //Resources
            DealDamage,
            MaxHealth,
            HealthRegen,
            ConsumeMagic,
            MaxMagic,
            MagicRegen,
            ExertStamina,
            MaxStamina,
            StaminaRegen,

            //Attributes
            Strength,
            Endurance,
            Intelligence,
            Wisdom,
            Dexterity,
            Agility,
            Speed,
            //Luck //is stupid...
        };

        enum ExpireType{
            Permanent, //Effects the stat permanently... spooky
            Equipped, //Does not expire until unequipped
            Timer, //Expires after time
            DistanceToPoint, //Expires if you leave a distance to a certain point
        };

        //Information for distance expiration
        struct DistanceExpiration{
            float cx;
            float cz;
            float d;
        };

        StatID statID;

        uint modifier;

        ExpireType expireType;

        union{
            float timeUntilExpiration;
            DistanceExpiration DistanceExpiration;
        };
    };

    //Resource
    class Resource{
    public:

        uint getFinal();
        uint getMax();

        void modifyRegenEffect( uint mod );
        void modifyMaxEffect( uint mod );

        void tick( float dt );

        void reset( uint max, uint regenPerSecond );

    protected:

        uint mBase; //Resource value
        uint mMax; //Base doesn't exceed max
        uint mRegenPerTick; //Amount Regened per tick

        uint mRegenModFromEffect; //Regen modded from effect
        uint mMaxModFromEffect; //Max modded from effect
    };

    //Attribute
    class Attribute{
    public:

        ushort getFinal();

        void modifyEffect( ushort mod );

        //Resets effects to 0 and base to passed in base
        void reset( ushort base );

    protected:

        ushort mBase;
        ushort mModFromEffect;
    };

    //Update: pass in time and character x and z
    void update( float dt, float cx, float cz );

    //Apply an effect to the stats
    void applyEffects( Effect* effect, uint effectCount );

    //Accessors

    inline Resource& getHealth();
    inline Resource& getMagic();
    inline Resource& getStamina();

    inline Attribute& getStrength();
    inline Attribute& getEndurance();
    inline Attribute& getIntelligence();
    inline Attribute& getWisdom();
    inline Attribute& getDexterity();
    inline Attribute& getAgility();
    inline Attribute& getSpeed();

protected:

    //Resources
    Resource mHealth;
    Resource mMagic;
    Resource mStamina;

    //Attributes
    Attribute mStrength;
    Attribute mEndurance;
    Attribute mIntelligence;
    Attribute mWisdom;
    Attribute mDexterity;
    Attribute mAgility;
    Attribute mSpeed;

    //Current applied effects
    Effect mActiveEffects[ STATS_MAX_ACTIVE_EFFECTS ];
};

#endif