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
            DroppedDown,
            SelectDropOption,
            MoveSlider,
            TextboxSelected
        };

        Action action;

        union{
            int dropOptionIndex;
            float sliderValue;
        };

        int id;
    };

    struct Text{
        char* message;
        XMFLOAT2 offset;
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

    enum SelectedState{
        Idle,
        Highlighted,
        Selected
    };

    //Constructor
    UIElement();

    //Destructor
    virtual ~UIElement();

    //Update, returning a change that has occured, if any
    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key ) = 0;

    //Set the position of the UI Element
    virtual void setPosition( XMFLOAT2& pos );
    virtual void setDimension( XMFLOAT2& dim );

    //Get Text from the element, to draw, for most elements it is only
    virtual void getText( Text** text, uint* textCount );

    //Set the text for a component
    virtual void setText( Text& text, uint index = 0 );

    //Get the position of the element
    inline const XMFLOAT2& getPosition();
    inline const XMFLOAT2& getDimension();

    //Accessors

    virtual ElemType getElemType() = 0;

    inline bool isActive();
    inline bool isHighlighted();

    inline void setActive( bool active );

    inline SelectedState getSelectedState();

protected:

    //Checks if a point is inside the component
    bool pointCheckInside( XMFLOAT2 );

protected:

    XMFLOAT2 mPosition;
    XMFLOAT2 mDimensions;

    bool mIsActive;
    SelectedState mSelected;

    Text mText;
};

inline bool UIElement::isActive(){return mIsActive;}
inline UIElement::SelectedState UIElement::getSelectedState(){return mSelected;}

inline void UIElement::setActive( bool active ){mIsActive = active;}

inline const XMFLOAT2& UIElement::getPosition(){return mPosition;}
inline const XMFLOAT2& UIElement::getDimension(){return mDimensions;}

class UIButton : public UIElement{
public:

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

    virtual ElemType getElemType(){return Button;}

protected:

    bool mClicked;
};

class UICheckbox : public UIElement{
public:

    UICheckbox();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

    virtual ElemType getElemType(){return CheckBox;}

    inline bool isChecked();
    inline void setChecked( bool val );

protected:

    bool mClicked;
    bool mChecked;
};

inline bool UICheckbox::isChecked(){return mChecked;}
inline void UICheckbox::setChecked( bool val ){mChecked = val;}

class UISlider : public UIElement{
public:

    UISlider();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

    void setPercent( float val );
    inline float getPercent();

    virtual ElemType getElemType(){return Slider;}

protected:
    
    float mPercent;
};

inline float UISlider::getPercent(){return mPercent;}

class UIDropMenu : public UIElement{
public:

    UIDropMenu();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

    void setOptions( char** options, uint optionCount );
    char* getOption( uint index );
    uint getOptionCount();

    virtual ElemType getElemType(){return DropMenu;}

    inline void setSelectedOption( uint option );
    inline uint getSelectedOption();
    inline uint getHightlightedOption();
    inline bool isDropped();

protected:

    char** mOptions;
    uint mOptionCount;

    uint mSelectedOption;
    uint mHighlightedOption;

    bool mIsDropped;
    bool mClicked;

    float mSavedHeight;
};

inline void UIDropMenu::setSelectedOption( uint option ){mSelectedOption = option;}
inline uint UIDropMenu::getSelectedOption(){return mSelectedOption;}
inline uint UIDropMenu::getHightlightedOption(){return mHighlightedOption;}
inline bool UIDropMenu::isDropped(){return mIsDropped;}

#define INPUTBOX_MAX_INPUT_LEN 128

class UIInputBox : public UIElement{
public:

    UIInputBox();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

    virtual ElemType getElemType(){return InputBox;}

    inline char* getInput();

protected:

    char mInput[ INPUTBOX_MAX_INPUT_LEN ];
    uint mCursor;
};

inline char* UIInputBox::getInput(){return mInput;}

class UITextbox : public UIElement{
public:

    UITextbox();

    virtual UserChange update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key );

protected:

};

#endif