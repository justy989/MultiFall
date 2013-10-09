#include "WorldScreen.h"

WorldScreen::WorldScreen( ScreenManager* screenManager, EventManager* eventManager, Party* party ) :
    Screen( screenManager, eventManager, party )
{
    mWindow.init(1);
    mWindow.initTab( 0, "", 3 );
    mWindow.setPosition( XMFLOAT2( -0.9f, 0.7f ) );
    mWindow.setDimension( XMFLOAT2( 0.3f, 0.25f ) );
    mWindow.setText( UIElement::Text( "World" ) );

    mHealthBar =  new UISlider();
    mHealthBar->setPosition( XMFLOAT2( 0.15f, 0.2f ) );
    mHealthBar->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mHealthBar->setText( UIElement::Text( "Health" ) );

    mMagicBar =  new UISlider();
    mMagicBar->setPosition( XMFLOAT2( 0.15f, 0.2f ) );
    mMagicBar->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mMagicBar->setText( UIElement::Text( "Magic" ) );

    mStaminaBar =  new UISlider();
    mStaminaBar->setPosition( XMFLOAT2( 0.15f, 0.2f ) );
    mStaminaBar->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mStaminaBar->setText( UIElement::Text( "Stamina" ) );

}

WorldScreen::~WorldScreen()
{
    mWindow.clear();
}

void WorldScreen::update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key )
{
    //UIElement::UserChange change = mWindow.update( mouseClick, mousePos, keyPress, key );

    if( keyPress ){
        if( key == 'P' ){
            mScreenManager->popScreen();
        }
    }

    uiDisplay->buildWindowVB( mWindow, aspectRatio );
}

void WorldScreen::draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager )
{
    uiDisplay->drawWindowText( device, mWindow, *textManager );
}