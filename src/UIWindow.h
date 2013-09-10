#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "UIElement.h"

#include <assert.h>

class UIWindow : public UIElement{
public:

    //Tab
    struct Tab{
        UIElement** elements;
        int elementCount;
        int size;

        char* name;

        float width;

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
    virtual void setPosition( XMFLOAT2 pos );

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

    inline int getCurrentTab();

    virtual ElemType getElemType(){return Window;}

protected:

    Tab* mTabs;
    int mTabCount;

    int mCurrentTab;

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

inline int UIWindow::getCurrentTab()
{
    return mCurrentTab;
}

#endif