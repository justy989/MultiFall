#include "ScreenManager.h"
#include "MenuScreen.h"
#include "LobbyScreen.h"
#include "WorldScreen.h"

ScreenManager::ScreenManager( EventManager* eventManager, Party* party )
{
    mMenuScreen = new MenuScreen( this, eventManager, party );
    mLobbyScreen = new LobbyScreen( this, eventManager, party );
    mWorldScreen = new WorldScreen( this, eventManager, party );
}

void ScreenManager::update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                            bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key )
{
    if( mScreens.size() ){
        mScreens.top()->update( dt, uiDisplay, aspectRatio, mouseClick, mousePos, keyPress, key );
    }
}

void ScreenManager::pushScreen( Type type )
{
    switch( type ){
    case Title:
        break;
    case Menu:
        mScreens.push( mMenuScreen );
        break;
    case Options:
        break;
    case Lobby:
        mScreens.push( mLobbyScreen );
        break;
    case World:
        mScreens.push( mWorldScreen );
        break;
    case Pause:
        break;
    default:
        break;
    }
}

void ScreenManager::popScreen()
{
    if( mScreens.size() ){
        mScreens.pop();
    }
}
