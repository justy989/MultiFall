#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "UIElement.h"

#include <assert.h>

#define UIWINDOW_TITLE_BAR_HEIGHT 0.1f

class UIWindow : public UIElement{
public:

    //Tab
    struct Tab{

        Tab();

        UIElement** elements;
        int elementCount;
        int size;

        char* name;

        float width;

        bool highlighted;

        XMFLOAT4 color;
    };

    //Constr
    UIWindow();

    //Destr
    virtual ~UIWindow();

    //Update func returns changes in ui elements
    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

    //Set the position of the UI Element
    virtual void setPosition( XMFLOAT2& pos );

    //Set the dimension
    virtual void setDimension( XMFLOAT2& dim );

    //Set the text for a component
    virtual void setText( Text& text );

    //Initialize the window to hold tabCount tabs
    void init( uint tabCount );

    //Intialize a tab to hold elemCount elements
    void initTab( uint tabIndex, char* name, uint elemCount );

    //Add an element to a tab
    void addElem( UIElement* element, uint tab = 0 );

    //Clear everything, delete allocations
    void clear();

    //Accessors

    inline UIElement** getTabElement(uint tabIndex, uint elemIndex);
    inline Tab& getTab(uint tabIndex);

    inline void setCurrentTab( uint tab );
    inline uint getCurrentTab();
    inline uint getTabCount();

    virtual ElemType getElemType(){return Window;}
    
    inline void setMinDimensions( XMFLOAT2 dim );

protected:

    Tab* mTabs;
    uint mTabCount;

    uint mCurrentTab;

    XMFLOAT2 mMinDimensions;

    XMFLOAT2 mTitleClickPos;
    XMFLOAT2 mResizeClickPos;

    static float mTitleBarHeight;
    static float mTabBarHeight;
};

inline UIElement** UIWindow::getTabElement(uint tabIndex, uint elemIndex)
{
    assert( tabIndex < mTabCount );
    assert( elemIndex < mTabs[ tabIndex ].elementCount );

    return mTabs[ tabIndex ].elements + elemIndex;
}

inline UIWindow::Tab& UIWindow::getTab(uint tabIndex )
{
    return mTabs[ tabIndex ];
}

inline uint UIWindow::getCurrentTab()
{
    return mCurrentTab;
}

inline void UIWindow::setCurrentTab( uint tab )
{
    assert( tab < mTabCount );
    mCurrentTab = tab;
}

inline void UIWindow::setMinDimensions( XMFLOAT2 dim )
{
    mMinDimensions = dim;
}

inline uint UIWindow::getTabCount()
{
    return mTabCount;
}

#endif