#include "LevelThemeLoader.h"
#include "WorldGenerator.h"
#include "WorldDisplay.h"

#include "Log.h"

#include <fstream>

LevelThemeLoader::LevelThemeLoader()
{

}

bool LevelThemeLoader::loadTheme( char* themePath, ID3D11Device* device, WorldGenerator* worldGen, WorldDisplay* worldDisplay )
{
    std::ifstream file( themePath );
    char buffer[128];

    if( !file.is_open() ){
        LOG_ERRO << "Failed to open theme: " << themePath << LOG_ENDL; 
        return false;
    }

    WCHAR wallTexturePath[128];
    WCHAR floorTexturePath[128];
    WCHAR ceilingTexturePath[128];
    Fog fog;
    float wallTextureClip = 1.0f;
    float floorTextureClip = 1.0f;
    float ceilingTextureClip = 1.0f;
    float objectDrawDistance = 1.0f;
    float lightDrawDistance = 1.0f;
    float characterDrawDistance = 1.0f;

    PointLight candle;
    PointLight torch;
    PointLight chandelier;

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
        }else if( strcmp( setting, "CeilingTexture" ) == 0  ){
            wsprintf(ceilingTexturePath, L"content/textures/");
            mbstowcs(ceilingTexturePath + 17, value, 128);
        }else if( strcmp( setting, "CeilingClip" ) == 0  ){
            ceilingTextureClip = static_cast<float>(atof( value ));
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
        }else if( strcmp( setting, "ObjectDrawDistance" ) == 0  ){
            objectDrawDistance = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "LightDrawDistance" ) == 0  ){
            lightDrawDistance = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "CharacterDrawDistance" ) == 0  ){
            characterDrawDistance = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "CandleRadius" ) == 0  ){
            candle.set( static_cast<float>(atof( value ) ), candle.getIntensity(), candle.getColor() );
        }else if( strcmp( setting, "CandleIntensity" ) == 0  ){
            candle.set( candle.getRadius(), static_cast<float>(atof( value ) ), candle.getColor() );
        }else if( strcmp( setting, "CandleColorRed" ) == 0  ){
            candle.getColor().x = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "CandleColorGreen" ) == 0  ){
            candle.getColor().y = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "CandleColorBlue" ) == 0  ){
            candle.getColor().z = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "TorchRadius" ) == 0  ){
            torch.set( static_cast<float>(atof( value ) ), torch.getIntensity(), torch.getColor() );
        }else if( strcmp( setting, "TorchIntensity" ) == 0  ){
            torch.set( torch.getRadius(), static_cast<float>(atof( value ) ), torch.getColor() );
        }else if( strcmp( setting, "TorchColorRed" ) == 0  ){
            torch.getColor().x = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "TorchColorGreen" ) == 0  ){
            torch.getColor().y = static_cast<float>(atof( value ));
        }else if( strcmp( setting, "TorchColorBlue" ) == 0  ){
            torch.getColor().z = static_cast<float>(atof( value ));
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

    fog.diff = fog.end - fog.start;
    fog.color.w = 1.0f;

    LevelDisplay* levelDisplay = &worldDisplay->getLevelDisplay();

    //Setup the level display fog
	worldDisplay->getLevelDisplay().setFog( fog );

    //Setup the draw distances for things
    worldDisplay->getLevelDisplay().setDrawRange( objectDrawDistance );
    worldDisplay->getLightDisplay().setDrawRange( lightDrawDistance );
    worldDisplay->getPopulationDisplay().setDrawRange( characterDrawDistance );

    //Set the light values
    worldDisplay->getLightDisplay().setPointLight( Level::Light::Candle, candle );
    worldDisplay->getLightDisplay().setPointLight( Level::Light::Torch, torch );
    //worldDisplay->getLightDisplay().setPointLight( Level::Light::Chandelier, chandelier );

    //Set the level display textures and clipping info
    return levelDisplay->setTextures( device, floorTexturePath, floorTextureClip, 
                                              wallTexturePath, wallTextureClip,
                                              ceilingTexturePath, ceilingTextureClip);
}