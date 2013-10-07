#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "EventManager.h"
#include <stack>

class ScreenManager;

class Screen{
public:

    Screen( ScreenManager* screenManager, EventManager* eventManager ) : 
        mEventManager(eventManager),
        mScreenManager(screenManager) {}

    //Init tscreen
    virtual void init( ) = 0;

    //Update screen
    virtual void update( float dt ) = 0;

    //shutdown screen
    virtual void shutdown( ) = 0;

protected:

    EventManager* mEventManager;
    ScreenManager* mScreenManager;
};

class ScreenManager{
public:

    //Init screens
    ScreenManager( EventManager* eventManager );

    //Type
    enum Type{
        Title,
        Menu,
        Options,
        Lobby,
        World,
        Pause
    };

    //Update the current state
    void update( float dt );

    //push the selected screen on top
    void pushScreen( Type type );

    //pop the current screen
    void popScreen();

protected:

    std::stack< Screen* > mScreens;
};

#endif