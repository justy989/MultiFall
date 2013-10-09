#include "ConnectionScreen.h"

ConnectionScreen::ConnectionScreen( ScreenManager* screenManager, EventManager* eventManager, Party* party ) :
    Screen( screenManager, eventManager, party )
{
    mWindow.init(1);
    mWindow.initTab( 0, "", 4 );
    mWindow.setPosition( XMFLOAT2( -0.3f, -0.5f ) );
    mWindow.setDimension( XMFLOAT2( 0.6f, 1.0f ) );
    mWindow.setText( UIElement::Text( "Connection" ) );

    mBackBtn =  new UIButton();
    mBackBtn->setPosition( XMFLOAT2( 0.15f, 0.2f ) );
    mBackBtn->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mBackBtn->setText( UIElement::Text( "Back" ) );

    mActionBtn =  new UIButton();
    mActionBtn->setPosition( XMFLOAT2( 0.15f, 0.4f ) );
    mActionBtn->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mActionBtn->setText( UIElement::Text( party->isLeader() ? "Create" : "Connect" ) );

    mIPBox = new UIInputBox();
    mIPBox->setPosition( XMFLOAT2( 0.15f, 0.4f ) );
    mIPBox->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mIPBox->setText( UIElement::Text( "Host", XMFLOAT2( -0.4f, 0.0f ) ) );

    mPortBox = new UIInputBox();
    mPortBox->setPosition( XMFLOAT2( 0.15f, 0.4f ) );
    mPortBox->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mPortBox->setText( UIElement::Text( "Port", XMFLOAT2( -0.4f, 0.0f ) ) );

    mWindow.addElem( mBackBtn );
    mWindow.addElem( mActionBtn );

    mWindow.addElem( mPortBox );

    if( !party->isLeader() ){
        mWindow.addElem( mIPBox );
    }else{
        delete mIPBox;
    }
}

ConnectionScreen::~ConnectionScreen()
{
    mWindow.clear();
}

void ConnectionScreen::update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key )
{
    UIElement::UserChange change = mWindow.update( mouseClick, mousePos, keyPress, key );

    if( change.action == UIElement::UserChange::Action::ClickButton ){
        switch( change.id ){
        case 0:
            mParty->disband();
            mScreenManager->popScreen();
            break;
        case 1:
            //Attempt to connect, if we are successful, 
            mScreenManager->pushScreen( ScreenManager::Type::World );
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

void ConnectionScreen::draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager )
{
    uiDisplay->drawWindowText( device, mWindow, *textManager );
}