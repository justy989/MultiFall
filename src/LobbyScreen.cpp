#include "LobbyScreen.h"

#include <time.h>

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
            {
                Event e;

                if( mReadyCheckbox->isChecked() ){
                    e.type = Event::Type::PartyMemberReady;
                    e.partyMemberReadyInfo.userIndex = mParty->getMyIndex();
                }else{
                    e.type = Event::Type::PartyMemberUnReady;
                    e.partyMemberUnReadyInfo.userIndex = mParty->getMyIndex();
                }

                EVENTMANAGER->queueEvent( e );
            }
            break;
        default:
            break;
        }
    }else if( change.action == UIElement::UserChange::Action::ClickButton ){
        switch( change.id ){
        case 2:
            {
                mParty->disband();

                Event e;
                e.type = Event::Type::PartyMemberLeave;
                e.partyMemberLeaveInfo.userIndex;
                EVENTMANAGER->queueEvent( e );

                mScreenManager->popScreen();
            }
            return;
        default:
            break;
        }
    }

    //If we are the party leader, check to see if everyone is ready!
    if( mParty->isLeader() ){
        int exists = 0;
        int ready = 0;
        for(int i = 0; i < PARTY_SIZE; i++){
            if( mParty->getMember(i).doesExist() ){
                exists++;
            }

            if( mParty->getMember(i).isReady() ){
                ready++;
            }
        }

        //Is everyone ready? Lets do this!
        if( exists == ready ){
            Event e;
            e.type = Event::Type::SessionStart;
            e.sessionStartInfo.levelSeed = time(0);
            EVENTMANAGER->queueEvent( e );
        }
    }

    char buffer[ 1024 ];
    buffer[ 0 ] = '\0';

    //Lets build the party Textbox
    for(int i = 0; i < PARTY_SIZE; i++){
        if( mParty->getMember(i).doesExist() ){
            sprintf(buffer, "%s\n%d: %s : %s", buffer, i, mParty->getMember(i).getName(), mParty->getMember(i).isReady() ? "Ready" : "Not Ready" );
        }
    }

    mPartyTextbox->setText( buffer, strlen( buffer ) );

    uiDisplay->buildWindowVB( mWindow, aspectRatio );
}

void LobbyScreen::draw( ID3D11DeviceContext* device, UIDisplay* uiDisplay, TextManager* textManager )
{
    uiDisplay->drawWindowText( device, mWindow, *textManager );
}

void LobbyScreen::handleEvent( Event& e )
{
    switch( e.type ){
    case Event::Type::SessionStart:
        mScreenManager->pushScreen( ScreenManager::Type::Worldy );
        break;
    default:
        break;
    }
}