#ifndef WORLDSCREEN_H
#define WORLDSCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class WorldScreen : public Screen{
public:

    WorldScreen( ScreenManager* screenManager, World* world, Party* party );
    ~WorldScreen();

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

    virtual void draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager );

protected:

    UIWindow mWindow;

    UISlider* mHealthBar;
    UISlider* mMagicBar;
    UISlider* mStaminaBar;
};

#endif