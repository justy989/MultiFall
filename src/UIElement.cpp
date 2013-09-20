#include "UIElement.h"

UIElement::UIElement() :
    mIsActive(true),
    mIsHighlighted(false)
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

    if( mouseClick && pointCheckInside( mousePosition ) ){
        change.action = UserChange::Action::ClickButton;
    }

    return change;
}

void UIButton::setPosition( XMFLOAT2& pos )
{
    mPosition = pos;

    centerText();
}

void UIButton::setDimension( XMFLOAT2& dim )
{
    mDimensions = dim;

    centerText();
}

void UIButton::setText( Text& text )
{
    mText = text;

    centerText();
}

void UIButton::centerText()
{

}

void UIButton::getText( Text** text, int* textCount )
{
    *textCount = 0;
    *text = &mText;
}