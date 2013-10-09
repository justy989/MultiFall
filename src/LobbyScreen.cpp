#include "LobbyScreen.h"

LobbyScreen::LobbyScreen( ScreenManager* screenManager, EventManager* eventManager, Party* party ) :
    Screen( screenManager, eventManager, party )
{
    mWindow.init(1);
    mWindow.initTab( 0, "", 2 );
    mWindow.setPosition( XMFLOAT2( -0.3f, -0.5f ) );
    mWindow.setDimension( XMFLOAT2( 0.6f, 1.0f ) );
    mWindow.setText( UIElement::Text( "Lobby" ) );

    char* msg = "Party Status Textbox";

    mPartyTextbox = new UITextBox();
    mPartyTextbox->setPosition( XMFLOAT2( -0.3f, -0.5f ) );
    mPartyTextbox->setDimension( XMFLOAT2( 0.6f, 1.0f ) );
    mPartyTextbox->setText( msg, strlen( msg ) ); 

    mWindow.addElem( mPartyTextbox );
}

LobbyScreen::~LobbyScreen()
{
    mWindow.clear();
}

void LobbyScreen::update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key )
{
    UIElement::UserChange change = mWindow.update( mouseClick, mousePos, keyPress, key );

    if( change.action == UIElement::UserChange::Action::ClickButton ){
        switch( change.id ){
        case 0:
            mParty->create();
            mScreenManager->pushScreen( ScreenManager::Type::Connection );
            break;
        case 1:
            mScreenManager->pushScreen( ScreenManager::Type::Connection );
            break;
        case 2:
            //Nothin yet!
            break;
        case 3:
            PostQuitMessage(0);
            break;
        default:
            break;
        }
    }

    uiDisplay->buildWindowVB( mWindow, aspectRatio );
}

void LobbyScreen::draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager )
{
    uiDisplay->drawWindowText( device, mWindow, *textManager );
}