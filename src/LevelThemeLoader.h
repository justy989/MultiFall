#ifndef LEVEL_THEME_LOADER_H
#define LEVEL_THEME_LOADER_H

class WorldGenerator;
class LevelDisplay;

#include <D3D11.h>

class LevelThemeLoader{
public:
    LevelThemeLoader();

	bool loadTheme( char* themePath, ID3D11Device* device, WorldGenerator* worldGen, LevelDisplay* levelDisplay );

protected:

};

#endif