#ifndef LobbySCREEN_H
#define LobbySCREEN_H

#include "UIWindow.h"
#include "ScreenManager.h"

class LobbyScreen : public Screen{
public:

    LobbyScreen( ScreenManager* screenManager, World* world, Party* party );
    ~LobbyScreen();

    //Update screen
    virtual void update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key );

    virtual void draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager );

protected:

    UIWindow mWindow;

    UITextBox* mPartyTextbox;
    UICheckbox* mReadyCheckbox;
    UIButton* mBackBtn;
};

#endif