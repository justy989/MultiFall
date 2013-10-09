#include "MenuScreen.h"

MenuScreen::MenuScreen( ScreenManager* screenManager, EventManager* eventManager ) :
    Screen( screenManager, eventManager )
{
    mWindow.init(1);
    mWindow.setPosition( XMFLOAT2( -0.3f, -0.5f ) );
    mWindow.setDimension( XMFLOAT2( 0.6f, 1.0f ) );
    mWindow.setText( UIElement::Text( "Main Menu" ) );

    mHostBtn =  new UIButton();
    mHostBtn->setPosition( XMFLOAT2( 0.15f, 0.2f ) );
    mHostBtn->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mHostBtn->setText( UIElement::Text( "Host" ) );

    mJoinBtn =  new UIButton();
    mJoinBtn->setPosition( XMFLOAT2( 0.15f, 0.2f ) );
    mJoinBtn->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mJoinBtn->setText( UIElement::Text( "Join" ) );

    mQuitBtn =  new UIButton();
    mQuitBtn->setPosition( XMFLOAT2( 0.15f, 0.2f ) );
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

void MenuScreen::init( )
{

}

void MenuScreen::update( float dt )
{

}

void MenuScreen::draw( UIDisplay* uiDisplay, TextManager* textManager, ID3D11DeviceContext* device )
{

}

void MenuScreen::shutdown( )
{

}