#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class MenuScreen : public Screen{
public:

    MenuScreen( ScreenManager* screenManager, EventManager* eventManager, Party* party );
    ~MenuScreen();

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

protected:

    UIWindow mWindow;

    UIButton* mHostBtn;
    UIButton* mJoinBtn;
    UIButton* mQuitBtn;
};

#endif