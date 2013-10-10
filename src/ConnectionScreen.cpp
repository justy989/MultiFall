#include "ConnectionScreen.h"

#include "Log.h"

ConnectionScreen::ConnectionScreen( ScreenManager* screenManager, World* world, Party* party ) :
    Screen( screenManager, world, party )
{
    mWindow.init(1);
    mWindow.initTab( 0, "", 5 );
    mWindow.setPosition( XMFLOAT2( -0.5f, -0.5f ) );
    mWindow.setDimension( XMFLOAT2( 1.0f, 1.0f ) );
    mWindow.setText( UIElement::Text( "Connection" ) );

    mBackBtn =  new UIButton();
    mBackBtn->setPosition( XMFLOAT2( 0.05f, 0.75f ) );
    mBackBtn->setDimension( XMFLOAT2( 0.4f, 0.12f ) );
    mBackBtn->setText( UIElement::Text( "Back" ) );

    mActionBtn =  new UIButton();
    mActionBtn->setPosition( XMFLOAT2( 0.5f, 0.75f ) );
    mActionBtn->setDimension( XMFLOAT2( 0.4f, 0.12f ) );
    mActionBtn->setText( UIElement::Text( party->isLeader() ? "Create" : "Connect" ) );

    mIPBox = new UIInputBox();
    mIPBox->setPosition( XMFLOAT2( 0.225f, 0.2f ) );
    mIPBox->setDimension( XMFLOAT2( 0.75f, 0.12f ) );
    mIPBox->setText( UIElement::Text( "Host", XMFLOAT2( -0.45f, 0.0f ) ) );

    mPortBox = new UIInputBox();
    mPortBox->setPosition( XMFLOAT2( 0.225f, 0.4f ) );
    mPortBox->setDimension( XMFLOAT2( 0.3f, 0.12f ) );
    mPortBox->setText( UIElement::Text( "Port", XMFLOAT2( -0.25f, 0.0f ) ) );

    mNameBox = new UIInputBox();
    mNameBox->setPosition( XMFLOAT2( 0.225f, 0.6f ) );
    mNameBox->setDimension( XMFLOAT2( 0.5f, 0.12f ) );
    mNameBox->setText( UIElement::Text( "Name", XMFLOAT2( -0.3f, 0.0f ) ) );

    mWindow.addElem( mBackBtn );
    mWindow.addElem( mActionBtn );

    mWindow.addElem( mPortBox );
    mWindow.addElem( mNameBox );

    if( !party->isLeader() ){
        mWindow.addElem( mIPBox );
    }else{
        delete mIPBox;
    }

    if( mParty->isLeader() ){
        mPortBox->setInput( "1990" );
        mNameBox->setInput( "Leader" );
    }else{
        mIPBox->setInput( "localhost" );
        mPortBox->setInput( "1990" );
        mNameBox->setInput( "Member" );
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
            return;
        case 1:
            {
                Event e;

                if( mParty->isLeader() ){
                    e.type = Event::Type::NetworkListen;
                    e.networkListenInfo.port = atoi( mPortBox->getInput() );
                    mParty->getMember(PARTY_LEADER_INDEX).setName( mNameBox->getInput() );
                }else{
                    e.type = Event::Type::NetworkConnect;
                    strcpy( e.networkConnectInfo.host, mIPBox->getInput() );
                    e.networkConnectInfo.port = atoi( mPortBox->getInput() );
                }

                EVENTMANAGER->queueEvent( e );
            }
            return;
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

void ConnectionScreen::handleEvent( Event& e )
{
    switch( e.type ){
    case Event::Type::NetworkIsConnected:
        {
            LOG_INFO << "Connected to host: " << mIPBox->getInput() << " : " << mPortBox->getInput() << LOG_ENDL;

            Event newEvent;
            newEvent.type = Event::Type::PartyJoinRequest;
            strcpy( newEvent.partyJoinRequestInfo.name, mNameBox->getInput() );
            EVENTMANAGER->queueEvent( newEvent );
        }
        break;
    case Event::Type::NetworkIsListening:
        LOG_INFO << "Listening on port: " << mPortBox->getInput() << LOG_ENDL;
        mScreenManager->pushScreen( ScreenManager::Type::Lobby );
        break;
    case Event::Type::NetworkDisconnect:
        LOG_INFO << "Failed to listen on specified port: " << mPortBox->getInput() << LOG_ENDL;
        break;
    case Event::Type::PartyJoinAccept:
        LOG_INFO << "Joined the party with the member index: " << e.partyJoinAcceptInfo.userIndex << LOG_ENDL;
        mParty->getMember( e.partyJoinAcceptInfo.userIndex ).setName( mNameBox->getInput() );
        mScreenManager->pushScreen( ScreenManager::Type::Lobby );
        break;
    default:
        break;
    }
}