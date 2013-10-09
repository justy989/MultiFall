#include "ScreenManager.h"

ScreenManager::ScreenManager( EventManager* eventManager )
{

}

void ScreenManager::update( float dt )
{
    if( mScreens.size() ){
        mScreens.top()->update( dt );
    }
}

void ScreenManager::draw( UIDisplay* uiDisplay, TextManager* textManager, ID3D11DeviceContext* device )
{
    if( mScreens.size() ){
        mScreens.top()->draw( uiDisplay, textManager, device );
    }
}

void ScreenManager::pushScreen( Type type )
{
    switch( type ){
    case Title:
        //mScreens.push( titleScreen );
        //mScreens.top()->init();
        break;
    case Menu:

        break;
    case Options:
        break;
    case Lobby:
        break;
    case World:
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
        mScreens.top()->shutdown();
        mScreens.pop();
    }
}
