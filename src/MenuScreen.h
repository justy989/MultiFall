#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class MenuScreen : public Screen{
public:

    MenuScreen( ScreenManager* screenManager, EventManager* eventManager );
    ~MenuScreen();

    //Init tscreen
    virtual void init( );

    //Update screen
    virtual void update( float dt );

    //Draw the screen
    virtual void draw( UIDisplay* uiDisplay, TextManager* textManager, ID3D11DeviceContext* device );

    //shutdown screen
    virtual void shutdown( );

protected:

    UIWindow mWindow;

    UIButton* mHostBtn;
    UIButton* mJoinBtn;
    UIButton* mQuitBtn;
};

#endif