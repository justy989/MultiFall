#include "UIWindow.h"

#include "TextManager.h"

float mTitleBarHeight = 0.1f;
float mTabBarHeight = 0.1f;

UIWindow::Tab::Tab() :
    elements(NULL),
    elementCount(0),
    name(NULL),
    width(0.0f),
    size(0),
    highlighted(false)
{

}

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

    
    float len = 0.0f;
    float lastLen = 0.0f;

    //Are we inside one of the tab locations?
    for(uint i = 0; i < mTabCount; i++){

        //Init highlighted to false
        mTabs[i].highlighted = false;

        len += static_cast<float>( strlen( mTabs[i].name ) ) * FONTWIDTH;

        if( mousePosition.x >= mPosition.x + lastLen && mousePosition.x <= mPosition.x + len &&
            mousePosition.y >= mPosition.y + UIWINDOW_TITLE_BAR_HEIGHT && 
            mousePosition.y <= mPosition.y + ( 2.0f * UIWINDOW_TITLE_BAR_HEIGHT ) ){
            if(mouseClick ){
                mCurrentTab = i;
            }else{
                mTabs[i].highlighted = true;
            }
        }

        lastLen = len;
    }

    for(int i = 0; i < mTabs[ mCurrentTab ].elementCount; i++){
        change = mTabs[ mCurrentTab ].elements[ i ]->update( mouseClick,
            XMFLOAT2( mousePosition.x - mPosition.x, mousePosition.y - mPosition.y ),
            keyPress, key );

        if( change.action != UserChange::Action::None ){
            change.id = i;
            return change;
        }
    }

    change.action = UserChange::Action::None;
    change.dropOptionIndex = 0;

    return change;
}

void UIWindow::setPosition( XMFLOAT2& pos )
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

void UIWindow::setDimension( XMFLOAT2& dim )
{
    mDimensions = dim;
}

void UIWindow::setText( Text& text )
{
    mText = text;
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

    //Init elements in tabs to NULL
    for(uint i = 0; i < elemCount; i++){
        mTabs[ tabIndex ].elements[i] = NULL;
    }
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
            for(int e = 0; e < mTabs[t].elementCount; e++){
                if( mTabs[t].elements[e] ){
                    delete[] mTabs[t].elements[e];
                }
            }

            delete[] mTabs[t].elements;
            mTabs[t].elements = NULL;
            mTabs[t].elementCount = 0;
        }

        delete[] mTabs;
        mTabs = NULL;
        mTabCount = 0;
    }
}