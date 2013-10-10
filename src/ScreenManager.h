#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "EventManager.h"
#include "UIDisplay.h"
#include "World.h"
#include "Party.h"
#include <stack>

class ScreenManager;

class Screen{
public:

    enum Transition{
        None,
        In,
        Running,
        Out,
        Done
    };

    Screen( ScreenManager* screenManager, World* world, Party* party ) : 
        mScreenManager(screenManager),
        mParty(party) {}

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key ) = 0;

    virtual void draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager ) = 0;

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

class ScreenManager : public EventHandler{
public:

    //Init screens
    ScreenManager( World* world, Party* party );

    //Type
    enum Type{
        Title,
        Menu,
        Options,
        Connection,
        Lobby,
        Worldy, //added a y at the end so it won't collide with World
        Pause
    };

    virtual void handleEvent( Event& e ) {}

    //Update the current state
    void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                 bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

    //push the selected screen on top
    void pushScreen( Type type );

    //draw the top screen
    void draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager );

    //pop the current screen
    void popScreen();

protected:

    std::stack< Screen* > mScreens;

    World* mWorld;
    Party* mParty;
};

#endif