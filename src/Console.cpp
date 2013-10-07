#include "App.h"
#include "Log.h"

#include <fstream>

Console::Console() : mLogUpdateTimer(0.0f), mCurHistoryIndex(-1) {}
Console::~Console() { mWindow.clear(); }

void Console::init( App* app )
{
    mApp = app;

    mWindow.init( 1 );

    mWindow.initTab( 0, "", 5 );

    XMFLOAT2 pos;
    XMFLOAT2 dim;

    pos.x = -0.95f;
    pos.y = -0.95f;

    dim.x = 1.9f;
    dim.y = 1.9f;

    mWindow.setPosition( pos );
    mWindow.setDimension( dim );

    UIElement::Text text;
    text.message = "Console";

    mWindow.setText( text );
    mWindow.setMinDimensions( dim );

    pos.x = 0.02f;
    pos.y = UIWINDOW_TITLE_BAR_HEIGHT + 0.01f;

    dim.x = 1.9f - 0.04f;
    dim.y = 1.65f;

    mTextbox = new UITextBox();
    mTextbox->setPosition( pos );
    mTextbox->setDimension( dim );

    pos.x = 0.15f;
    pos.y = 1.775f;

    dim.x = 1.9f - 0.04f;
    dim.y = FONTHEIGHT + 0.06f;

    text.message = "CMD:";
    text.offset.x = -( dim.x / 2.0f ) - FONTWIDTH * 5.0f + 0.15f;
    text.offset.y = 0.0f;

    mInputBox = new UIInputBox();
    mInputBox->setPosition( pos );
    mInputBox->setDimension( dim );
    mInputBox->setText( text );
    mInputBox->setSelectedstate( UIElement::SelectedState::Selected );

    mWindow.addElem( mTextbox, 0 );
    mWindow.addElem( mInputBox, 0 );
    //mWindow.addElem( mSendBtn, 0 );
    //mWindow.addElem( mScrollUpBtn, 0 );
    //mWindow.addElem( mScrollDownBtn, 0 );

    for(int i = 0; i < CONSOLE_HISTORY_COUNT; i++){
        mHistory[i][0] = '\0';
    }
}

void Console::update( float dt, bool mouseClick, XMFLOAT2 mousePosition, bool keyPress, byte key )
{
    mLogUpdateTimer += dt;

    if( mLogUpdateTimer > CONSOLE_LOG_UPDATE_THRESH ){
        mLogUpdateTimer = 0.0f;
        updateLog();
    }

    if( keyPress ){

        if( key == VK_RETURN ){

            //Make sure a string has been entered
            if( strlen( mInputBox->getInput() ) ){

                //Add to history
                addToHistory( mInputBox->getInput() );

                //Execute command
                sendCmd( mInputBox->getInput() );

                //Clear the box
                mInputBox->clearInput();

                //Reset cur history index
                mCurHistoryIndex = -1;
            }
        }

        if( key == VK_UP ){
            scrollHistory( true );
        }else if( key == VK_DOWN ){
            scrollHistory( false );
        }
    }

    if( key != VK_RETURN && key != VK_UP && key != VK_DOWN && key != 192 ){
        UIElement::UserChange change = mWindow.update( mouseClick, mousePosition, keyPress, key );
    }
}

void Console::updateLog()
{
    std::ifstream file("multifall.log");

    if( !file.is_open() ){
        return;
    }

    char buffer[CONSOLE_SAVED_LINES][256];

    for(int i = 0; i < CONSOLE_SAVED_LINES; i++){
        buffer[i][0] = '\0';
    }

    while( !file.eof() ){

        //Loop through and push back all the lines, forgetting the last one
        for(int i = CONSOLE_SAVED_LINES - 1; i >= 1; i--){
            strcpy( buffer[i], buffer[i-1] );
        }

        file.getline( buffer[0], 256 );
    }

    int minExpectedLines = static_cast<int>( mTextbox->getDimension().y / ( FONTHEIGHT * 1.5f ) );
    int wordWrappedLines = CONSOLE_SAVED_LINES;
    int linesUsed = CONSOLE_SAVED_LINES - 1;
    char fullText[ 4096 ];

    while( wordWrappedLines > minExpectedLines ){
        fullText[0] = '\0';

        for(int i = linesUsed; i > 0; i--){
            //Offset buffer ptr by timestamp and msg type: '12:17:05 Info: ' 15 characters 
            sprintf(fullText, "%s%s%c", fullText, buffer[i] + 15, '\n' );
        }
 
        wordWrappedLines = mTextbox->setText( fullText, strlen( fullText ) );
        linesUsed--;
    }

    file.close();
}

void Console::sendCmd( char* cmd )
{
    char* op;
    char* args[ 8 ];
    int argCount = 0;
    bool help = false;

    op = strtok( cmd, " " );

    do{
        args[argCount] = strtok( NULL, " " );
        argCount++;

        if( argCount >= 8 ){
            break;
        }
    }while( args[argCount-1] );

    //Help?
    if( strcmp( op, "help" ) == 0 ){
        help = true;
        if( argCount >= 2 ){
            op = args[0];
        }else{
            LOG_INFO << "Type 'help (command from list)' to get info on a command" << LOG_ENDL;
            LOG_INFO << "Calling a command without arguments generally prints info" << LOG_ENDL;
            LOG_INFO << "Commands: " << LOG_ENDL;
            LOG_INFO << "print camfov fogcolor fogrange genlevel spawn kill" << LOG_ENDL;
            return;
        }
    }

    argCount--;

    //Print
    if( strcmp( op, "print" ) == 0 ){
        if( help ){
            LOG_INFO << "Print arguments given to the log" << LOG_ENDL;
        }else{
            char buffer[256];
            buffer[0] = '\0';

            for(int i = 0; i < argCount; i++){
                sprintf(buffer, "%s%s", buffer, args[i]);
            }

            LOG_INFO << buffer << LOG_ENDL;
        }

    //Set the field of view
    }else if( strcmp( op, "camfov" ) == 0 ){
        if( help ){
            LOG_INFO << "Sets the camera's Y field of view" << LOG_ENDL;
        }else{
            if( argCount ){
                mApp->mCamera.setFOV( atof( args[0] ) );
            }else{
                LOG_INFO << "Camera Y FOV: " << mApp->mCamera.getFOV() << LOG_ENDL;
            }
        }
    }else if( strcmp( op, "fogcolor" ) == 0 ){
        if( help ){
            LOG_INFO << "Sets the color of the fog using Red Green Blue as floats" << LOG_ENDL;
        }else{
            if( argCount == 3 ){
                Fog& f = mApp->mWorldDisplay.getLevelDisplay().getFog();

                f.color.x = atof( args[0] );
                f.color.y = atof( args[1] );
                f.color.z = atof( args[2] );

            }else{
                Fog& f = mApp->mWorldDisplay.getLevelDisplay().getFog();
                LOG_INFO << "Fog: R: " << f.color.x << " G: " << f.color.y << " B: " << f.color.z << LOG_ENDL;
            }
        }
    }else if( strcmp( op, "fogrange" ) == 0 ){
        if( help ){
            LOG_INFO << "Sets the range of the fog from: start to end" << LOG_ENDL;
        }else{
            if( argCount == 2 ){
                Fog& f = mApp->mWorldDisplay.getLevelDisplay().getFog();

                f.start = atof( args[0] );
                f.end = atof( args[1] );
                f.diff = f.end - f.start;

            }else{
                Fog& f = mApp->mWorldDisplay.getLevelDisplay().getFog();
                LOG_INFO << "Fog: Start: " << f.start << " End: " << f.end << LOG_ENDL;
            }
        }
    }else if(strcmp( op, "genlevel" ) == 0 ){
        if( help ){
            LOG_INFO << "Generates the level from scratch given a seed." << LOG_ENDL;
        }else{
            if( argCount == 1 ){
                mApp->mWorldGen.genLevel( mApp->mWorld.getLevel(), mApp->mLevelGenRanges, atoi( args[0] ), mApp->mBlockDimenions );
                mApp->mWorldGen.genPopulation( mApp->mWorld.getPopulation(), mApp->mWorld.getLevel(), mApp->mPopGenRanges, mApp->mBlockDimenions );
                mApp->mWorldDisplay.getLevelDisplay().createMeshFromLevel( mApp->mWindow.getDevice(), mApp->mWorld.getLevel(), mApp->mBlockDimenions, mApp->mBlockDimenions );
            }else{
                LOG_INFO << "Provide 1 argument for the seed to genlevel" << LOG_ENDL;
            }
        }
    }else if(strcmp( op, "spawn" ) == 0 ){
        if( help ){
            LOG_INFO << "Spawns a character at the specified location: X, Y" << LOG_ENDL;
        }else{
            if( argCount == 2 ){
                Event e;
                e.type = Event::Type::CharacterSpawn;
                e.characterSpawnInfo.id = 0;
                e.characterSpawnInfo.x = atof( args[0] );
                e.characterSpawnInfo.z = atof( args[1] );
                mApp->mEventManager.enqueueEvent( e );
            }else{
                LOG_INFO << "Proved 2 arguments for the x and z positions of the character" << LOG_ENDL;
            }
        }
    }else if(strcmp( op, "kill" ) == 0 ){
        if( help ){
            LOG_INFO << "Kills a character at the specified index" << LOG_ENDL;
        }else{
            if( argCount == 1 ){
                Event e;
                e.type = Event::Type::CharacterKill;
                e.characterSpawnInfo.id = atoi( args[0] );
                mApp->mEventManager.enqueueEvent( e );
            }else{
                LOG_INFO << "Proved 2 arguments for the x and z positions of the character" << LOG_ENDL;
            }
        }
    }else{
        LOG_INFO << "Unrecognized command: " << op << LOG_ENDL;
    }
}

void Console::addToHistory( char* cmd )
{
    //Forget the last entry
    for(int i = CONSOLE_HISTORY_COUNT - 1; i > 0; i--){
        strcpy( mHistory[ i ], mHistory[ i - 1 ] );
    }

    strcpy( mHistory[ 0 ], cmd );
}

void Console::scrollHistory( bool up )
{
    mCurHistoryIndex += up ? 1 : -1;

    if( mCurHistoryIndex <= -1 ){
        mInputBox->clearInput();
        mCurHistoryIndex = -1;
        return;
    }

    CLAMP( mCurHistoryIndex, 0, CONSOLE_HISTORY_COUNT - 1 );

    if( !strlen( mHistory[ mCurHistoryIndex ] ) ){
        return;
    }

    mInputBox->setInput( mHistory[ mCurHistoryIndex ] );
}