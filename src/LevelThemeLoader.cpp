#include "LevelThemeLoader.h"
#include "WorldGenerator.h"
#include "LevelDisplay.h"

#include "Log.h"

#include <fstream>

LevelThemeLoader::LevelThemeLoader()
{

}

bool LevelThemeLoader::loadTheme( char* themePath, ID3D11Device* device, WorldGenerator* worldGen, LevelDisplay* levelDisplay )
{
    std::ifstream file( themePath );
    char buffer[128];

    if( !file.is_open() ){
        LOG_ERRO << "Failed to open theme: " << themePath << LOG_ENDL; 
        return false;
    }

    WCHAR wallTexturePath[128];
    WCHAR floorTexturePath[128];
    Fog fog = { XMFLOAT4(0,0,0,1), 1.0f, 10.0f };
    float wallTextureClip = 1.0f;
    float floorTextureClip = 1.0f;

    while( !file.eof() ){
        file.getline( buffer, 128 );

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

        if( strcmp( setting, "WallTexture" ) == 0 ){
            wsprintf(wallTexturePath, L"content/textures/");
            mbstowcs(wallTexturePath + 17, value, 128);
        }else if( strcmp( setting, "WallClip" ) == 0  ){
            wallTextureClip = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "FloorTexture" ) == 0  ){
            wsprintf(floorTexturePath, L"content/textures/");
            mbstowcs(floorTexturePath + 17, value, 128);
        }else if( strcmp( setting, "FloorClip" ) == 0  ){
            floorTextureClip = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "FogStart" ) == 0  ){
            fog.start = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "FogEnd" ) == 0  ){
            fog.end = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "FogColorRed" ) == 0  ){
            fog.color.x = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "FogColorGreen" ) == 0  ){
            fog.color.y = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "FogColorBlue" ) == 0  ){
            fog.color.z = static_cast<float>(atof( value ));
        }else{
            LOG_ERRO << "Unknown Theme Configuration Setting: " << setting << LOG_ENDL;
        }
    }

    //Close the file
    file.close();

    //Calculate the number of rows/collumns in the texture, assuming they are the same
    int floorRows = static_cast<int>(1.0f / floorTextureClip);

    //Set the worldGen max Tile ID to generate
    worldGen->setTileIDMax( floorRows * floorRows );

    //Setup the level display fog
    levelDisplay->setFog( fog.color, fog.start, fog.end );

    //Set the level display textures and clipping info
    return levelDisplay->setTextures( device, floorTexturePath, floorTextureClip, wallTexturePath, wallTextureClip );
}