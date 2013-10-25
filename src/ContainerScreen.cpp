#include "ContainerScreen.h"

ContainerScreen::ContainerScreen( ScreenManager* screenManager, World* world, Party* party ) :
    Screen( screenManager, world, party )
{
    mWindow.init(1);
    mWindow.initTab( 0, "", 6 );
    mWindow.setPosition( XMFLOAT2( -0.75f, -0.75f ) );
    mWindow.setDimension( XMFLOAT2( 1.5f, 1.5f ) );
    mWindow.setText( UIElement::Text( "Container" ) );

    mItemBox = new UIOptionBox();


    char buffer[256];

    /*
    for(int i = 0; i < 12; i++){
        sprintf(buffer, "%d", i ); 
        mItemBox->addOption(buffer);
    }*/

    WorldContainer* container = world->getOpenedContainer();

    if( container ){
        for(uint i = 0; i < container->getNumItems(); i++){
            ItemDefinition* item = world->getItemMaster().getDefinitionFromID( container->getItem(i)->id );
            mItemBox->addOption( item->name );
        }
    }

    sprintf(buffer, "%d Items", container->getNumItems() );

    mItemBox->setText( UIElement::Text(buffer, XMFLOAT2( 0.0f, -FONTHEIGHT - 0.04f ) ) );

    mItemBox->setPosition( XMFLOAT2( 0.05f, 0.225f ) );
    mItemBox->setDimension( XMFLOAT2( 0.75f, 1.05f ) );

    mItemBox->setScrollLimit( 48 );
    mItemBox->setScrollIndex( 0 );

    mTextBox = new UITextBox();

    mTextBox->setPosition( XMFLOAT2( 0.825f, 0.325f ) );
    mTextBox->setDimension( XMFLOAT2( 0.65f, 0.875f ) );

    container = mWorld->getOpenedContainer();
    if( container ){
        ItemDefinition* item = mWorld->getItemMaster().getDefinitionFromID( container->getItem(mItemBox->getSelected())->id );
        ItemDefinition::buildStringFromItem(buffer, item, container->getItem(mItemBox->getSelected())->stack); 
    }

    mTextBox->setText( buffer, strlen(buffer) );

    mTakeButton = new UIButton();

    mTakeButton->setText( UIElement::Text("Take") );
    mTakeButton->setPosition( XMFLOAT2( 0.35f, 1.375f ) );
    mTakeButton->setDimension( XMFLOAT2( 0.2f, 0.08f ) );

    mBackButton = new UIButton();

    mBackButton->setText( UIElement::Text("Back") );
    mBackButton->setPosition( XMFLOAT2( 0.1f, 1.375f ) );
    mBackButton->setDimension( XMFLOAT2( 0.2f, 0.08f ) );

    mScrollUpBtn = new UIButton();

    mScrollUpBtn->setText( UIElement::Text("Up") );
    mScrollUpBtn->setPosition( XMFLOAT2( 0.825f, 0.225f ) );
    mScrollUpBtn->setDimension( XMFLOAT2( 0.15f, 0.08f ) );

    mScrollDownBtn = new UIButton();

    mScrollDownBtn->setText( UIElement::Text("Down") );
    mScrollDownBtn->setPosition( XMFLOAT2( 0.825f, 1.2f ) );
    mScrollDownBtn->setDimension( XMFLOAT2( 0.15f, 0.08f ) );

    mWindow.addElem( mItemBox );
    mWindow.addElem( mBackButton );
    mWindow.addElem( mTakeButton );
    mWindow.addElem( mScrollUpBtn );
    mWindow.addElem( mScrollDownBtn );
    mWindow.addElem( mTextBox );
}

ContainerScreen::~ContainerScreen()
{
    mWindow.clear();
}

void ContainerScreen::update( float dt, UIDisplay* uiDisplay, float aspectRatio,
                         bool mouseClick, XMFLOAT2 mousePos, bool keyPress, byte key )
{
    UIElement::UserChange change = mWindow.update( mouseClick, mousePos, keyPress, key );

    if( change.action == UIElement::UserChange::ClickButton ){
        if( change.id == 1 ){
            Event e;
            e.type = Event::ContainerClose;
            e.containerCloseInfo.memberIndex = mParty->getMyIndex();
            EVENTMANAGER->queueEvent(e);
            mScreenManager->popScreen();
            return;
        }else if( change.id == 2 ){

        }else if( change.id == 3 ){
            /*
            mItemBox->scroll( true );

            mItemBox->clearOptions();

            char buffer[ 32 ];

            for(int i = 0; i < 12; i++){
                sprintf(buffer, "%d", i + ( mItemBox->getScrollIndex() - 12 ) ); 
                mItemBox->addOption(buffer);
            }*/

        }else if( change.id == 4 ){
            /*
            mItemBox->scroll( false );

            char buffer[ 32 ];

            mItemBox->clearOptions();

            for(int i = 0; i < 12; i++){
                sprintf(buffer, "%d", i + ( mItemBox->getScrollIndex() - 12 ) ); 
                mItemBox->addOption(buffer);
            }*/
        }
    }

    char buffer[256];

    WorldContainer* container = mWorld->getOpenedContainer();
    ItemDefinition* item = mWorld->getItemMaster().getDefinitionFromID( container->getItem(mItemBox->getSelected())->id );
    ItemDefinition::buildStringFromItem(buffer, item, container->getItem(mItemBox->getSelected())->stack);

    mTextBox->setText( buffer, strlen(buffer) );

    uiDisplay->buildWindowVB( mWindow, aspectRatio );
}

void ContainerScreen::draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager )
{
    uiDisplay->drawWindowText( device, mWindow, *textManager );
}

void ContainerScreen::handleEvent( Event& e )
{
    
}