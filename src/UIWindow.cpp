#include "UIWindow.h"

float mTitleBarHeight = 0.1f;
float mTabBarHeight = 0.1f;

UIWindow::UIWindow() :
    mTabs(NULL),
    mTabCount(0),
    mCurrentTab(0)
{
    mTitleClickPos.x = -2.0f;
    mResizeClickPos.x = -2.0f;

    mMinDimensions.x = 0.25f;
    mMinDimensions.y = 0.25f;
}

UIWindow::~UIWindow()
{
    clear();
}

UIWindow::UserChange UIWindow::update( bool mouseClick, XMFLOAT2 mousePosition, 
                                       bool keyPress, byte key )
{
    UserChange change;

    //Has it clicked down?
    if( mouseClick ){
        //Have we saved the mouse position?
        if( mTitleClickPos.x > -1.1f ){
            //Move the window based on how far the mouse position has changed
            mPosition.x += mousePosition.x - mTitleClickPos.x;
            mPosition.y += mousePosition.y - mTitleClickPos.y;
            mTitleClickPos = mousePosition;
        }else{ //If not we need to
            if( mousePosition.x >= mPosition.x && mousePosition.x <= (mPosition.x + mDimensions.x) &&
                mousePosition.y >= mPosition.y && mousePosition.y <= (mPosition.y + UIWINDOW_TITLE_BAR_HEIGHT) ){
                mTitleClickPos = mousePosition;
            }
        }

        //Have we saved the mouse position?
        if( mResizeClickPos.x > -1.1f ){
            //Move the window based on how far the mouse position has changed
            mDimensions.x += mousePosition.x - mResizeClickPos.x;
            mDimensions.y += mousePosition.y - mResizeClickPos.y;

            CLAMP( mDimensions.x, mMinDimensions.x, 2.0f );
            CLAMP( mDimensions.y, mMinDimensions.y, 2.0f );

            mResizeClickPos = mousePosition;
        }else{ //If not we need to
            if( mousePosition.x >= (mPosition.x + mDimensions.x - 0.1f) && mousePosition.x <= (mPosition.x + mDimensions.x) &&
                mousePosition.y >= (mPosition.y + mDimensions.y - 0.1f) && mousePosition.y <= (mPosition.y + mDimensions.y) ){
                mResizeClickPos = mousePosition;
            }
        }
    }else{
        mTitleClickPos.x = -2.0f;
        mResizeClickPos.x = -2.0f;
    }



    /*
    for(int i = 0; i < mTabs[ mCurrentTab ].elementCount; i++){
        change = mTabs[ mCurrentTab ].elements[ i ]->update( mouseClick,
            mousePosition,
            keyPress, key );

        if( change.action != UserChange::Action::None ){
            return change;
        }
    }*/

    change.action = UserChange::Action::None;
    change.dropOptionIndex = 0;

    return change;
}

void UIWindow::setPosition( XMFLOAT2 pos )
{
    /*
    //Loops through tabs and each element in them. Set the position
    for(int t = 0; t < mTabCount; t++){
        for(int e = 0; e < mTabs[ t ].elementCount; e++){
            mTabs[ t ].elements[ e ]->setPosition( pos );
        }
    }

    //Adjust the text position as well
    mText.position.x = pos.x + 0.01f;
    mText.position.y = pos.y + 0.01f;
    */
    mPosition = pos;
}

void UIWindow::setDimension( XMFLOAT2 dim )
{
    mDimensions = dim;
}

void UIWindow::setText( Text& text )
{
    mText = text;

    //Position the text
    mText.position.x = mPosition.x + 0.01f;
    mText.position.y = mPosition.y + 0.01f;
}

void UIWindow::init( uint tabCount )
{
    clear();

    mTabs = new Tab[ tabCount ];
    mTabCount = tabCount;
}

void UIWindow::initTab( uint tabIndex, char* name, uint elemCount )
{
    assert( tabIndex < mTabCount );
    assert( mTabs[ tabIndex ].elements == NULL );

    mTabs[ tabIndex ].elements = new UIElement*[ elemCount ];
    mTabs[ tabIndex ].elementCount = 0;
    mTabs[ tabIndex ].size = elemCount;
    mTabs[ tabIndex ].name = name;
}

void UIWindow::addElem( UIElement* element, uint tabIndex )
{
    assert( tabIndex < mTabCount );
    assert( mTabs[ tabIndex ].elementCount < mTabs[ tabIndex ].size );

    mTabs[ tabIndex ].elements[ mTabs[ tabIndex ].elementCount ] = element;
    mTabs[ tabIndex ].elementCount++;
}

void UIWindow::clear()
{
    if( mTabs ){
        for(int t = 0; t < mTabCount; t++){
            delete[] mTabs[t].elements;
            mTabs[t].elements = NULL;
            mTabs[t].elementCount = 0;
        }

        delete[] mTabs;
        mTabs = NULL;
        mTabCount = 0;
    }
}