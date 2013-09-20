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

    //Read the wall texture
    file >> buffer;
    WCHAR wallTexturePath[128];
    wsprintf(wallTexturePath, L"content/textures/");
    mbstowcs(wallTexturePath + 17, buffer, 128);
    
    //Read the wall clip
    file >> buffer;
    float wallTextureClip = static_cast<float>(atof( buffer ));

    //Wrap up to the end of the line
    file.getline(buffer, 128);

    //Read the wall texture
    file >> buffer;
    WCHAR floorTexturePath[128];
    wsprintf(floorTexturePath, L"content/textures/");
    mbstowcs(floorTexturePath + 17, buffer, 128);
    
    //Read the wall clip
    file >> buffer;
    float floorTextureClip = static_cast<float>(atof( buffer ));

    //Wrap up to the end of the line
    file.getline(buffer, 128);

    int floorRows = static_cast<int>(1.0f / floorTextureClip);

    file.close();

    worldGen->setTileIDMax( floorRows * floorRows );

    return levelDisplay->setTextures( device, floorTexturePath, floorTextureClip, wallTexturePath, wallTextureClip );;
}