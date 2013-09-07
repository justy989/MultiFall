#ifndef DUNGENAPP_H
#define DUNGENAPP_H

#include "Dx11Window.h"
#include "Timer.h"
#include "Camera.h"

#include "WorldDisplay.h"
#include "World.h"

#include "Configuration.h"
#include "UserBinds.h"

#include "TextManager.h"

#include "WorldEntity.h"
#include "WorldGenerator.h"

#include <d3dx11effect.h>

class App : public WMInputHandler{
public:

    App();

    //handle input from the dx11 window
    virtual void handleInput( RAWINPUT* input );

    //run the application
    int run( HINSTANCE hInstance, int nCmdShow );

protected:

    bool init( );

    void update( float dt );
    void draw( );

    void clear( );

protected:

    Configuration mConfig;

    UserBinds mBinds;

    DX11Window mWindow;

    Timer mTimer;

    Camera mCamera;

    WorldDisplay mWorldDisplay;

    World mWorld;

	TextManager mTextManager;

    //Tmp
    bool camKeyDown[4];
    bool wireframe;

    bool collisionMode;

    char FPSString[128];
    char CameraPosString[128];
    char collidedString[128];

    ID3D11RasterizerState* mFillRS;
    ID3D11RasterizerState* mWireFrameRS;

    WorldEntity mEntity;

    WorldGenerator::LevelPreset mLevelPreset;
};

#endif