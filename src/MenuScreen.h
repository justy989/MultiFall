#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class MenuScreen : public Screen{
public:

    MenuScreen( ScreenManager* screenManager, World* world, Party* party );
    ~MenuScreen();

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

    virtual void draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager );

protected:

    UIWindow mWindow;

    UIButton* mHostBtn;
    UIButton* mJoinBtn;
    UIButton* mOptionsBtn;
    UIButton* mQuitBtn;
};

#endif