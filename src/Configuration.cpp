#include "Configuration.h"
#include "Log.h"

#include <fstream>
//#include <rapidxml.hpp>
//#include <rapidxml_utils.hpp>

Configuration::Configuration()
{
    //Setup default values for all settings
    mWindow.width = 800;
    mWindow.height = 600;
    mWindow.hardwareRendered = 1;

    mSound.masterVolume = 1.0f;
    mSound.musicVolume = 1.0f;
    mSound.FXVolume = 1.0f;

    mBindNames[0] = "PlayerMoveForward"; mBinds[ 0 ] = 'W';
    mBindNames[1] = "PlayerMoveBackward"; mBinds[ 1 ] = 'S';
    mBindNames[2] = "PlayerMoveLeft"; mBinds[ 2 ] = 'A';
    mBindNames[3] = "PlayerMoveRight"; mBinds[ 3 ] = 'D';
    mBindNames[4] = "PlayerJump";
    mBindNames[5] = "PlayerCrouch";
    mBindNames[6] = "PlayerAttack";
    mBindNames[7] = "PlayerCast";
    mBindNames[8] = "PlayerActivate";

    mBindNames[9] = "MenuPause";
    mBindNames[10] = "MenuSelect";
    mBindNames[11] = "MenuCancel";
    mBindNames[12] = "MenuCycleForward";
    mBindNames[13] = "MenuCycleBack";

    mLongKeyNames[0].key = VK_SPACE;
    mLongKeyNames[0].longName = "SPACE";

    mLongKeyNames[1].key = VK_LCONTROL;
    mLongKeyNames[1].longName = "LCTRL";

    mLongKeyNames[2].key = VK_LSHIFT;
    mLongKeyNames[2].longName = "LSHIFT";

    mLongKeyNames[3].key = VK_RCONTROL;
    mLongKeyNames[3].longName = "RCTRL";

    mLongKeyNames[4].key = VK_RSHIFT;
    mLongKeyNames[4].longName = "RSHIFT";
   
    mLongKeyNames[5].key = VK_TAB;
    mLongKeyNames[5].longName = "TAB";

    mLongKeyNames[6].key = VK_MENU;
    mLongKeyNames[6].longName = "ALT";

    mLongKeyNames[7].key = VK_RETURN;
    mLongKeyNames[7].longName = "ENTER";

    mLongKeyNames[8].key = VK_LBUTTON;
    mLongKeyNames[8].longName = "LCLICK";

    mLongKeyNames[9].key = VK_RBUTTON;
    mLongKeyNames[9].longName = "RCLICK";

    mSensitivity = 0.001f;
}

bool Configuration::load( char* configPath )
{
    std::ifstream checkFile( configPath );
    char buffer[ 128 ];

    enum LoadState{
        Window,
        SoundVolume,
        UserBinds,
        Misc,
        YourMotherTrebek
    };

    LoadState ls;

    if(!checkFile.is_open()){
        LOG_ERRO << "Configuration file doesn't exist: " << configPath << LOG_ENDL;
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

        //Check to see if we are changing modes
        if( strcmp( setting, "[Window]" ) == 0 ){
            ls = Window;
            continue;
        }else if( strcmp( setting, "[SoundVolume]" ) == 0 ){
            ls = SoundVolume;
            continue;
        }else if( strcmp( setting, "[UserBinds]" ) == 0 ){
            ls = UserBinds;
            continue;
        }else if( strcmp( setting, "[Misc]" ) == 0 ){
            ls = Misc;
            continue;
        }

        switch( ls ){

        case UserBinds:
            {
                bool foundBind = false;
                for(int i = 0; i < USERBIND_ACTION_COUNT; i++){
                    if( strcmp( setting, mBindNames[i] ) == 0 ){
                        //TODO: Take into account long names

                        bool foundLongName = false;
                        //Check to see if it is a long name first
                        for( int c = 0; c < CONFIGURATION_LONG_KEY_NAME_COUNT; c++){
                            if( strcmp( value, mLongKeyNames[c].longName ) == 0 ){
                                mBinds[i] = mLongKeyNames[c].key;
                                foundLongName = true;
                                break;
                            }
                        }

                        //If not, then just take the first character
                        if( !foundLongName ){
                            mBinds[i] = value[0];
                        }

                        
                        LOG_INFO << "Configured User Bind: " << mBindNames[i] << " : " << mBinds[i] << LOG_ENDL;

                        foundBind = true;
                        break;
                    }
                }

                if( !foundBind ){
                    LOG_ERRO << "Unknown UserBind Setting: " << setting << LOG_ENDL;
                }
            }
            break;

        case Window:
            if( strcmp( setting, "Width" ) == 0 ){
                mWindow.width = atoi( value );
                LOG_INFO << "Configured Window Width: " << mWindow.width << LOG_ENDL;
            }else if( strcmp( setting, "Height" ) == 0  ){
                mWindow.height = atoi( value );
                LOG_INFO << "Configured Window Height: " << mWindow.height << LOG_ENDL;
            }else if( strcmp( setting, "HardwareRendered" ) == 0  ){
                mWindow.hardwareRendered = atoi( value );
                LOG_INFO << "Configured Window Hardware Rendered: " << mWindow.hardwareRendered << LOG_ENDL;
            }else{
                LOG_ERRO << "Unknown Window Setting: " << setting << LOG_ENDL;
            }

            break;
        
        case SoundVolume:
            if( strcmp( setting, "Master" ) == 0  ){
                mSound.masterVolume = static_cast<float>(atof( value ));
                LOG_INFO << "Configured Sound Master Volume: " << mSound.masterVolume << LOG_ENDL;
            }else if( strcmp( setting, "Music" ) == 0  ){
                mSound.musicVolume = static_cast<float>(atof( value ));
                LOG_INFO << "Configured Sound Music Volume: " << mSound.musicVolume << LOG_ENDL;
            }else if( strcmp( setting, "FX" ) == 0  ){
                mSound.FXVolume = static_cast<float>(atof( value ));
                LOG_INFO << "Configured Sound FX Volume: " << mSound.FXVolume << LOG_ENDL;
            }else{
                LOG_ERRO << "Unknown Sound Setting: " << setting << LOG_ENDL;
            }

            break;
  
        case Misc:
            if( strcmp( setting, "Sensitivity" ) == 0 ){
                mSensitivity = static_cast<float>(atof( value ));
                LOG_INFO << "Configured Sensitivity: " << mSensitivity << LOG_ENDL;
            }else{
                LOG_ERRO << "Unknown Misc Setting: " << setting << LOG_ENDL;
            }
            break;
        default:
            break;
        }
    }

    checkFile.close();

    //rapidxml::file<> xmlFile(configPath);
    //rapidxml::xml_document<> doc;
    //doc.parse<0>(xmlFile.data());

    return true;
}

void Configuration::save( char* configPath )
{

}