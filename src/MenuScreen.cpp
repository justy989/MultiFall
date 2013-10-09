#include "MenuScreen.h"

MenuScreen::MenuScreen( ScreenManager* screenManager, EventManager* eventManager, Party* party ) :
    Screen( screenManager, eventManager, party )
{
    mWindow.init(1);
    mWindow.initTab( 0, "", 3 );
    mWindow.setPosition( XMFLOAT2( -0.3f, -0.5f ) );
    mWindow.setDimension( XMFLOAT2( 0.6f, 1.0f ) );
    mWindow.setText( UIElement::Text( "Main Menu" ) );

    mHostBtn =  new UIButton();
    mHostBtn->setPosition( XMFLOAT2( 0.15f, 0.2f ) );
    mHostBtn->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mHostBtn->setText( UIElement::Text( "Host" ) );

    mJoinBtn =  new UIButton();
    mJoinBtn->setPosition( XMFLOAT2( 0.15f, 0.4f ) );
    mJoinBtn->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mJoinBtn->setText( UIElement::Text( "Join" ) );

    mQuitBtn =  new UIButton();
    mQuitBtn->setPosition( XMFLOAT2( 0.15f, 0.6f ) );
    mQuitBtn->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mQuitBtn->setText( UIElement::Text( "Quit" ) );

    mWindow.addElem( mHostBtn );
    mWindow.addElem( mJoinBtn );
    mWindow.addElem( mQuitBtn );
}

MenuScreen::~MenuScreen()
{
    mWindow.clear();
}

void MenuScreen::update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key )
{
    UIElement::UserChange change = mWindow.update( mouseClick, mousePos, keyPress, key );

    if( change.action == UIElement::UserChange::Action::ClickButton ){
        switch( change.id ){
        case 0:
            mParty->create();
            mScreenManager->pushScreen( ScreenManager::Type::Lobby );
            break;
        case 1:
            mScreenManager->pushScreen( ScreenManager::Type::Lobby );
            break;
        case 2:
            PostQuitMessage(0);
            break;
        default:
            break;
        }
    }

    uiDisplay->buildWindowVB( mWindow, aspectRatio );
}