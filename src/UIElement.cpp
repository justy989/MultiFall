#include "UIElement.h"

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

void UIButton::setPosition( XMFLOAT2& pos )
{
    mPosition = pos;
}

void UIButton::setDimension( XMFLOAT2& dim )
{
    mDimensions = dim;
}

void UIButton::setText( Text& text )
{
    mText = text;
}

void UIButton::getText( Text** text, int* textCount )
{
    *textCount = 0;
    *text = &mText;
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

void UISlider::setPosition( XMFLOAT2& pos )
{
    mPosition = pos;
}

void UISlider::setDimension( XMFLOAT2& dim )
{
    mDimensions = dim;
}

void UISlider::getText( Text** text, int* textCount )
{
    *textCount = 0;
    *text = &mText;
}

void UISlider::setText( Text& text )
{
    mText = text;
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

void UICheckbox::setPosition( XMFLOAT2& pos )
{
    mPosition = pos;
}

void UICheckbox::setDimension( XMFLOAT2& dim )
{
    mDimensions = dim;
}

void UICheckbox::getText( Text** text, int* textCount )
{
    *textCount = 0;
    *text = &mText;
}

void UICheckbox::setText( Text& text )
{
    mText = text;
}