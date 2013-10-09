#ifndef WORLDSCREEN_H
#define WORLDSCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class WorldScreen : public Screen{
public:

    WorldScreen( ScreenManager* screenManager, EventManager* eventManager, Party* party );
    ~WorldScreen();

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

protected:

    UIWindow mWindow;

    UISlider* mHealthBar;
    UISlider* mMagicBar;
    UISlider* mStaminaBar;
};

#endif