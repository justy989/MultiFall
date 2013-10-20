#ifndef CONTAINERSCREEN_H
#define CONTAINERSCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class ContainerScreen : public Screen{
public:

    ContainerScreen( ScreenManager* screenManager, World* world, Party* party );
    ~ContainerScreen();

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

    virtual void draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager );

protected:

    UIWindow mWindow;

    UIOptionBox* mItemBox;

    UIButton* mBackButton;
    UIButton* mTakeButton;

    UIButton* mScrollUpBtn;
    UIButton* mScrollDownBtn;
};

#endif