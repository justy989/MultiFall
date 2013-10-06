#ifndef CONSOLE_H
#define CONSOLE_H

#include "UIWindow.h"
#include "World.h"
#include "Camera.h"
#include "WorldDisplay.h"
#include "WorldGenerator.h"

#define CONSOLE_LOG_UPDATE_THRESH 0.5f
#define CONSOLE_SAVED_LINES 32
#define CONSOLE_HISTORY_COUNT 32

class App;

class Console{
public:

    Console();
    ~Console();

    void init( App* app );

    void update( float dt, bool mouseClick, XMFLOAT2 mousePosition, bool keyPress, byte key );

    inline UIWindow& getWindow();

protected:

    void updateLog();
    void sendCmd( char* cmd );
    void addToHistory( char* cmd );
    void scrollHistory( bool up );

protected:
    UIWindow mWindow;

    UITextBox* mTextbox;
    UIInputBox* mInputBox;
    UIButton* mSendBtn;
    UIButton* mScrollUpBtn;
    UIButton* mScrollDownBtn;

    float mLogUpdateTimer;

    App* mApp;

    char mHistory[ CONSOLE_HISTORY_COUNT ][ INPUTBOX_MAX_INPUT_LEN ];
    int mCurHistoryIndex;
};

inline UIWindow& Console::getWindow(){return mWindow;}

#endif