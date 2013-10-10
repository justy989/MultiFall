#ifndef CONNECTIONSCREEN_H
#define CONNECTIONSCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class ConnectionScreen : public Screen{
public:

    ConnectionScreen( ScreenManager* screenManager, World* world, Party* party );
    ~ConnectionScreen();

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

    virtual void draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager );

    virtual void handleEvent( Event& e );

protected:

    UIWindow mWindow;

    UIInputBox* mIPBox;
    UIInputBox* mPortBox;
    UIInputBox* mNameBox;
    UIButton* mBackBtn;
    UIButton* mActionBtn;
};

#endif