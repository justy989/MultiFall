#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "UserBinds.h"

#define CONFIGURATION_LONG_KEY_NAME_COUNT 10

class Configuration{
public:

    struct Window{
        int width;
        int height;
        int hardwareRendered;
    };

    struct Sound{
        float masterVolume;
        float musicVolume;
        float FXVolume;
    };

    Configuration();

    bool load( char* configPath );
    void save( char* configPath );

    inline Window getWindow();
    inline Sound getSound();

    inline char* getBinds();
    inline float getSensitivity();

protected:

    struct LongKeyName{
        char* longName;
        char key;
    };

    Window mWindow;
    Sound mSound;
    float mSensitivity;
    char mBinds[ USERBIND_ACTION_COUNT ];

    char* mBindNames[ USERBIND_ACTION_COUNT ];
    LongKeyName mLongKeyNames[CONFIGURATION_LONG_KEY_NAME_COUNT ];
};

inline Configuration::Window Configuration::getWindow(){return mWindow;}
inline Configuration::Sound Configuration::getSound(){return mSound;}

inline char* Configuration::getBinds(){return mBinds;}
inline float Configuration::getSensitivity(){return mSensitivity;}

#endif