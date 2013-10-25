#include "WorldScreen.h"

WorldScreen::WorldScreen( ScreenManager* screenManager, World* world, Party* party ) :
    Screen( screenManager, world, party )
{
    mWindow.init(1);
    mWindow.initTab( 0, "", 3 );
    mWindow.setPosition( XMFLOAT2( -0.99f, 0.56f ) );
    mWindow.setDimension( XMFLOAT2( 0.35f, 0.43f ) );
    mWindow.setText( UIElement::Text( "Status" ) );

    mHealthBar =  new UISlider();
    mHealthBar->setPosition( XMFLOAT2( 0.025f, 0.12f ) );
    mHealthBar->setDimension( XMFLOAT2( 0.3f, 0.08f ) );
    mHealthBar->setText( UIElement::Text( "H" ) );
    mHealthBar->setPercent( 0.5f );

    mMagicBar =  new UISlider();
    mMagicBar->setPosition( XMFLOAT2( 0.025f, 0.22f ) );
    mMagicBar->setDimension( XMFLOAT2( 0.3f, 0.08f ) );
    mMagicBar->setText( UIElement::Text( "M" ) );
    mMagicBar->setPercent( 0.8f );

    mStaminaBar =  new UISlider();
    mStaminaBar->setPosition( XMFLOAT2( 0.025f, 0.32f ) );
    mStaminaBar->setDimension( XMFLOAT2( 0.3f, 0.08f ) );
    mStaminaBar->setText( UIElement::Text( "S" ) );
    mStaminaBar->setPercent( 0.2f );

    mWindow.addElem( mHealthBar );
    mWindow.addElem( mMagicBar );
    mWindow.addElem( mStaminaBar );
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
        if( key == 'L' ){
            mScreenManager->popScreen();
            return;
        }/*else if( key == 'C' ){
            mScreenManager->pushScreen( ScreenManager::Container );
        }*/
    }

    uiDisplay->buildWindowVB( mWindow, aspectRatio );
}

void WorldScreen::draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager )
{
    uiDisplay->drawWindowText( device, mWindow, *textManager );
}

void WorldScreen::handleEvent( Event& e )
{
    if( e.type == Event::ContainerOpen ){
        if( mWorld->getOpenedContainer() ){
            mScreenManager->pushScreen( ScreenManager::Container );
        }
    }
}