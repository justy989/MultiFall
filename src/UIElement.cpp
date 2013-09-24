#include "UIElement.h"
#include "TextManager.h"

#include <assert.h>

UIElement::UIElement() :
    mIsActive(true),
    mSelected(SelectedState::Idle)
{
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;

    mDimensions.x = 0.0f;
    mDimensions.y = 0.0f;
}


UIElement::~UIElement()
{

}

bool UIElement::pointCheckInside( XMFLOAT2 point )
{
    return ( point.x >= mPosition.x && point.x <= ( mPosition.x + mDimensions.x ) &&
             point.y >= mPosition.y && point.y <= ( mPosition.y + mDimensions.y ) );
}


void UIElement::setPosition( XMFLOAT2& pos )
{
    mPosition = pos;
}

void UIElement::setDimension( XMFLOAT2& dim )
{
    mDimensions = dim;
}

void UIElement::getText( Text** text, uint* textCount )
{
    *textCount = 0;
    *text = &mText;
}

void UIElement::setText( Text& text, uint index )
{
    mText = text;
}

UIElement::UserChange UIButton::update( bool mouseClick, XMFLOAT2 mousePosition, 
                                        bool keyPress, byte key )
{
    UserChange change;
    change.action = UserChange::Action::None;

    mSelected = SelectedState::Idle;

    //Is the mouse inside the button?
    if( pointCheckInside( mousePosition ) ){

        //Is the mouse clicked?
        if( mouseClick ){
            //Tell the display the button is selected
            mSelected = SelectedState::Selected;

            //Only send event if this is the first click
            if( !mClicked ){
                change.action = UserChange::Action::ClickButton;
                mClicked = true;
            }
        }else{
            //Highlight the button
            mSelected = SelectedState::Highlighted;
        }
    }

    //Reset the mClicked when the mouse is released
    if( !mouseClick ){
        mClicked = false;
    }

    return change;
}

UISlider::UISlider() :
    mPercent(0.0f)
{

}

UIElement::UserChange UISlider::update( bool mouseClick, XMFLOAT2 mousePosition, 
                            bool keyPress, byte key )
{
    UIElement::UserChange change;
    change.action = UIElement::UserChange::Action::None;

    mSelected = UIElement::SelectedState::Idle;

    if( pointCheckInside( mousePosition ) ){
        mSelected = UIElement::SelectedState::Highlighted;

        if( mouseClick ){
            setPercent( (mousePosition.x - mPosition.x) / (mDimensions.x) );
            change.action = UIElement::UserChange::Action::MoveSlider;
        }
    }

    return change;
}

void UISlider::setPercent( float val )
{
    CLAMP( val, 0.0f, 1.0f );
    mPercent = val;
}

UICheckbox::UICheckbox() :
    mChecked(false)
{

}

UIElement::UserChange UICheckbox::update( bool mouseClick, XMFLOAT2 mousePosition, 
                            bool keyPress, byte key )
{
    UIElement::UserChange change;
    change.action = UIElement::UserChange::None;

    mSelected = UIElement::SelectedState::Idle;

    if( pointCheckInside( mousePosition ) ){
        mSelected = UIElement::SelectedState::Highlighted;

        if( mouseClick ){
            if( !mClicked ){
                change.action = UserChange::Action::CheckBox;
                mClicked = true;
                mChecked = !mChecked;
            }
        }
    }

    //Reset the mClicked when the mouse is released
    if( !mouseClick ){
        mClicked = false;
    }

    return change;
}

UIDropMenu::UIDropMenu() :
    mOptions(NULL),
    mOptionCount(0),
    mIsDropped(false),
    mSelectedOption(0),
    mHighlightedOption(0),
    mSavedHeight(0.0f),
{

}

UIDropMenu::UserChange UIDropMenu::update( bool mouseClick, XMFLOAT2 mousePosition, 
                                           bool keyPress, byte key )
{
    UIElement::UserChange change;
    change.action = UIElement::UserChange::None;

    mSelected = UIElement::SelectedState::Idle;

    if( !mIsDropped ){
        if( pointCheckInside( mousePosition ) ){
            mSelected = UIElement::SelectedState::Highlighted;

            if( mouseClick ){
                if( !mClicked ){
                    change.action = UserChange::Action::DroppedDown;
                    mClicked = true;
                    mIsDropped = true;
                    mSavedHeight = mDimensions.y;
                    mDimensions.y = FONTHEIGHT * static_cast<float>(mOptionCount);
                }
            }
        }
    }else{
        if( pointCheckInside( mousePosition ) ){
            mSelected = UIElement::SelectedState::Highlighted;

            if( mouseClick ){
                if( !mClicked ){
                    change.action = UserChange::Action::SelectDropOption;
                    mSelectedOption = static_cast<uint>( ( mousePosition.y - mPosition.y ) / FONTHEIGHT );
                }
            }
        }
    }

    //Reset the mClicked when the mouse is released
    if( !mouseClick ){
        mClicked = false;
    }

    return change;
}

void UIDropMenu::setOptions( char** options, uint optionCount )
{
    mOptions = options;
    mOptionCount = optionCount;
}

char* UIDropMenu::getOption( uint index )
{
    assert( index < mOptionCount );

    return mOptions[ index ];
}

uint UIDropMenu::getOptionCount()
{
    return mOptionCount;
}