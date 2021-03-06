#include "ScreenManager.h"
#include "MenuScreen.h"
#include "LobbyScreen.h"
#include "WorldScreen.h"
#include "ContainerScreen.h"
#include "ConnectionScreen.h"

ScreenManager::ScreenManager( World* world, Party* party )
{
    mWorld = world;
    mParty = party;
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
        mScreens.push( new MenuScreen( this, mWorld, mParty ) );
        break;
    case Options:
        break;
    case Connection:
        mScreens.push( new ConnectionScreen( this, mWorld, mParty ) );
        break;
    case Lobby:
        mScreens.push( new LobbyScreen( this, mWorld, mParty ) );
        break;
    case Worldy:
        mScreens.push( new WorldScreen( this, mWorld, mParty ) );
        break;
    case Pause:
        break;
    case Container:
        mScreens.push( new ContainerScreen( this, mWorld, mParty ) );
        break;
    default:
        break;
    }
}

void ScreenManager::popScreen()
{
    if( mScreens.size() ){
        Screen* screen = mScreens.top();
        mScreens.pop();
        delete screen;
    }
}

void ScreenManager::draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager )
{
    if( mScreens.size() ){
        mScreens.top()->draw( device, uiDisplay, textManager );
    }
}