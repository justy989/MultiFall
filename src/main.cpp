#include "App.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    //Create the app and run it
    App app;

    //Run it Run it
    return app.run( hInstance, nCmdShow );
}