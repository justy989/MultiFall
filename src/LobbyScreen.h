#ifndef LOBBYSCREEN_H
#define LOBBYSCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class LobbyScreen : public Screen{
public:

    LobbyScreen( ScreenManager* screenManager, EventManager* eventManager, Party* party );
    ~LobbyScreen();

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

protected:

    UIWindow mWindow;

    UIInputBox* mIPBox;
    UIInputBox* mPortBox;
    UIButton* mBackBtn;
    UIButton* mActionBtn;
};

#endif