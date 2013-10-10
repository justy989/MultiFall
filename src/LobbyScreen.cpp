#include "LobbyScreen.h"

LobbyScreen::LobbyScreen( ScreenManager* screenManager, World* world, Party* party ) :
    Screen( screenManager, world, party )
{
    mWindow.init(1);
    mWindow.initTab( 0, "", 3 );
    mWindow.setPosition( XMFLOAT2( -0.5f, -0.6f ) );
    mWindow.setDimension( XMFLOAT2( 1.0f, 1.4f ) );
    mWindow.setText( UIElement::Text( "Lobby" ) );

    char* msg = "Party Status Textbox";

    mPartyTextbox = new UITextBox();
    mPartyTextbox->setPosition( XMFLOAT2( 0.05f, 0.15f ) );
    mPartyTextbox->setDimension( XMFLOAT2( 0.9f, 1.0f ) );
    mPartyTextbox->setText( msg, strlen( msg ) ); 

    mReadyCheckbox = new UICheckbox();
    mReadyCheckbox->setPosition( XMFLOAT2( 0.8f, 1.2f ) );
    mReadyCheckbox->setDimension( XMFLOAT2( 0.1f, 0.1f ) );
    mReadyCheckbox->setText( UIElement::Text("Ready?", XMFLOAT2( -0.2f, 0.0f ) ) );

    mBackBtn = new UIButton();
    mBackBtn->setPosition( XMFLOAT2( 0.05f, 1.2f ) );
    mBackBtn->setDimension( XMFLOAT2( 0.3f, 0.1f ) );
    mBackBtn->setText( UIElement::Text("Back" ) );

    mWindow.addElem( mPartyTextbox );
    mWindow.addElem( mReadyCheckbox );
    mWindow.addElem( mBackBtn );
}

LobbyScreen::~LobbyScreen()
{
    mWindow.clear();
}

void LobbyScreen::update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key )
{
    UIElement::UserChange change = mWindow.update( mouseClick, mousePos, keyPress, key );

    if( change.action == UIElement::UserChange::Action::CheckBox ){
        switch( change.id ){
        case 1:
            mScreenManager->pushScreen( ScreenManager::Type::Worldy );
            break;
        default:
            break;
        }
    }else if( change.action == UIElement::UserChange::Action::ClickButton ){
        switch( change.id ){
        case 2:
            //Go back 2 states!
            mScreenManager->popScreen();
            return;
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