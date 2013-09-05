#include "App.h"
#include "Log.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    //Create the app and run it
    App app;

    Log::setFilePath("multifall.log");

    //Run it Run it
    int rc = app.run( hInstance, nCmdShow );

    LOG_INFO << "Goodbye cruel world of MultiFall" << LOG_ENDL;

    Log::destroyInst();

    return rc;
}