#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "EventManager.h"
#include "UIDisplay.h"
#include "Party.h"
#include <stack>

class ScreenManager;
class MenuScreen;
class LobbyScreen;
class WorldScreen;

class Screen{
public:

    enum Transition{
        None,
        In,
        Running,
        Out,
        Done
    };

    Screen( ScreenManager* screenManager, EventManager* eventManager, Party* party ) : 
        mEventManager(eventManager),
        mScreenManager(screenManager),
        mParty(party) {}

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key ) = 0;

    //Transition into the screen
    void transitionIn( );

    //Transition out of the screen
    void transitionOut( );

    inline Transition getTransition();

protected:

    EventManager* mEventManager;
    ScreenManager* mScreenManager;
    Party* mParty;

    Transition mTransition;
};

inline Screen::Transition Screen::getTransition(){return mTransition;}

class ScreenManager{
public:

    //Init screens
    ScreenManager( EventManager* eventManager, Party* party );

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
    void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                 bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

    //push the selected screen on top
    void pushScreen( Type type );

    //pop the current screen
    void popScreen();

protected:

    std::stack< Screen* > mScreens;

    MenuScreen* mMenuScreen;
    LobbyScreen* mLobbyScreen;
    WorldScreen* mWorldScreen;
};

#endif