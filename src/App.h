#ifndef DUNGENAPP_H
#define DUNGENAPP_H

#include "Dx11Window.h"
#include "Timer.h"
#include "Camera.h"

#include "WorldDisplay.h"
#include "World.h"

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

    void update( );
    void draw( );

    void clear( );

protected:

    DX11Window mWindow;

    Timer mTimer;

    Camera mCamera;

    WorldDisplay mWorldDisplay;

    World mWorld;

    //Tmp
    bool camKeyDown[4];
    bool wireframe;

    ID3D11RasterizerState* mFillRS;
    ID3D11RasterizerState* mWireFrameRS;
};

#endif