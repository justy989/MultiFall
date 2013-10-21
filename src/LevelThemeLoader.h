#ifndef LEVEL_THEME_LOADER_H
#define LEVEL_THEME_LOADER_H

class WorldGenerator;
class WorldDisplay;

#include <D3D11.h>

class LevelThemeLoader{
public:
    LevelThemeLoader();

	bool loadTheme( char* themePath, ID3D11Device* device, WorldGenerator* worldGen, WorldDisplay* worldDisplay );

protected:

};

#endif