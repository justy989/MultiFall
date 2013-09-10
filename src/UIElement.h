#ifndef UIElement_H
#define UIElement_H

#include "Utils.h"


class UIElement{
public:
    
    struct UserChange{
        enum Action{
            None,
            ClickButton,
            CheckBox,
            UncheckBox,
            SelectDropOption,
            MoveSlider,
            TextboxSelected
        };

        Action action;

        union{
            int dropOptionIndex;
            float sliderValue;
        };
    };

    struct Text{
        char* message;
        XMFLOAT2 position;
    };

    enum ElemType{
        None,
        Button,
        CheckBox,
        Slider,
        DropMenu,
        InputBox,
        TextBox,
        Window
    };

    //Constructor
    UIElement();

    //Destructor
    virtual ~UIElement();

    //Update, returning a change that has occured, if any
    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key ) = 0;

    //Set the position of the UI Element
    virtual void setPosition( XMFLOAT2 pos ) = 0;

    //Get Text from the element, to draw
    virtual void getText( Text** text, int* textCount ){
        *text = &mText;
        *textCount = 0;
    }

    //Set the text for a component
    virtual void setText( Text& text ) = 0;

    //Get the position of the element
    inline const XMFLOAT2& getPosition();

    //Accessors

    virtual ElemType getElemType() = 0;

    inline bool isActive();
    inline bool isHighlighted();

    inline void setActive( bool active );

protected:

    //Checks if a point is inside the component
    bool pointCheckInside( XMFLOAT2 );

protected:

    XMFLOAT2 mPosition;
    XMFLOAT2 mDimensions;

    bool mIsActive;
    bool mIsHighlighted;

    Text mText;
};

inline bool UIElement::isActive(){return mIsActive;}
inline bool UIElement::isHighlighted(){return mIsHighlighted;}

inline void UIElement::setActive( bool active ){mIsActive = active;}

inline const XMFLOAT2& UIElement::getPosition(){return mPosition;}

class UIButton : public UIElement{
public:

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

    virtual void setPosition( XMFLOAT2 pos );
    virtual void getText( Text** text, int* textCount );
    virtual void setText( Text& text );

    virtual ElemType getElemType(){return Button;}

protected:

    void centerText();
};

class UICheckbox : public UIElement{
public:

    UICheckbox();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

protected:

};

class UISlider : public UIElement{
public:

    UISlider();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

protected:

};

class UIDropMenu : public UIElement{
public:

    UIDropMenu();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

protected:

};

class UIInputBox : public UIElement{
public:

    UIInputBox();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

protected:

};

class UITextbox : public UIElement{
public:

    UITextbox();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

protected:

};

#endif