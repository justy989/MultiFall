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
    mSavedHeight(0.0f)
{

}

UIDropMenu::UserChange UIDropMenu::update( bool mouseClick, XMFLOAT2 mousePosition, 
                                           bool keyPress, byte key )
{
    UIElement::UserChange change;
    change.action = UIElement::UserChange::None;

    mSelected = UIElement::SelectedState::Idle;

    //If we aren't dropped
    if( !mIsDropped ){

        //If we have the mouse inside the box
        if( pointCheckInside( mousePosition ) ){
            mSelected = UIElement::SelectedState::Highlighted;

            //If the mouse is clicked
            if( mouseClick ){
                if( !mClicked ){

                    //Set the action
                    change.action = UserChange::Action::DroppedDown;
                    mClicked = true; //Has been clicked, avoids spamming action
                    mIsDropped = true; //Set the state to dropped
                    mSavedHeight = mDimensions.y; //Save the original dimension
                    mDimensions.y = mSavedHeight * static_cast<float>(mOptionCount); //Expand by fontheight * optionCount
                }
            }
        }
    }else{ //If we have dropped, the user wants to select an option

        //If the click is inside the mouse position
        if( pointCheckInside( mousePosition ) ){
            mSelected = UIElement::SelectedState::Highlighted;
            mHighlightedOption = static_cast<uint>( ( mousePosition.y - mPosition.y ) / mSavedHeight );

            //If the mouse is clicked
            if( mouseClick ){
                if( !mClicked ){
                    //Set the action
                    change.action = UserChange::Action::SelectDropOption;

                    //Determine which option is selected
                    mSelectedOption = static_cast<uint>( ( mousePosition.y - mPosition.y ) / mSavedHeight );

                    //Reset to collapsed state
                    mIsDropped = false;
                    mDimensions.y = mSavedHeight;

                    mClicked = true;
                }
            }
        }else{

            //IF the click is outside the box
            if( mouseClick ){
                //Undrop the box
                mDimensions.y = mSavedHeight;
                mIsDropped = false;
                mClicked = true;
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

UIInputBox::UIInputBox() :
    mCursor(0)
{
    //Init first char to NULL terminator
    mInput[0] = '\0';
}

UIElement::UserChange UIInputBox::update( bool mouseClick, XMFLOAT2 mousePosition, 
                               bool keyPress, byte key )
{
    UIElement::UserChange change;
    change.action = UIElement::UserChange::None;

    if( keyPress ){
        if( mSelected == UIElement::SelectedState::Selected ){
            if( key == VK_BACK ){
                if( mCursor > 0 ){
                    mCursor--;
                    mInput[ mCursor ] = '\0';
                }
            }else if( key == VK_OEM_PERIOD ){
                    mInput[ mCursor ] = '.'; 
                    mCursor++;
                    mInput[ mCursor ] = '\0';
            }else{
                if( mCursor < (INPUTBOX_MAX_INPUT_LEN - 1) ){

                    if( key >= 'A' && key <= 'Z' ){
                        key += 32;
                    }

                    mInput[ mCursor ] = key; 
                    mCursor++;
                    mInput[ mCursor ] = '\0';
                }
            }
        }
    }

    if( pointCheckInside( mousePosition ) ){
        if( mouseClick ){
            mSelected = UIElement::SelectedState::Selected;
        }
    }else{
        if( mouseClick ){
            mSelected = UIElement::SelectedState::Idle;
        }
    }

    return change;
}

UITextBox::UITextBox() :
    mLineCount(0)
{

}

UIElement::UserChange UITextBox::update( bool mouseClick, XMFLOAT2 mousePosition, 
                            bool keyPress, byte key )
{
    //Nothin to do broski, its a textboxxxxxxxxx
    UIElement::UserChange change;
    change.action = UIElement::UserChange::Action::None;
    return change;
}

void UITextBox::getText( Text** text, uint* textCount )
{
    *text = mTextLines;
    *textCount = mLineCount;
}

int UITextBox::setText( char* text, uint len )
{
    //Make a copy of the text in order to insert new lines
    strcpy( mFullText, text );

    uint lineLen = static_cast<int>( mDimensions.x / FONTWIDTH );
    uint textItr = 0;

    mLineCount = 0;

    //The best kind of inifite loops, while( true ) is overrated
    while( textItr < len ){

        bool foundNewline = false;

        //Loop through the line first checking for newlines
        for( uint c = textItr; c <= (textItr + lineLen); c++){
            if( c >= len ){
                mTextLines[ mLineCount ].message = mFullText + textItr;
                mTextLines[ mLineCount ].offset.x = 0.0f;
                mTextLines[ mLineCount ].offset.y = mLineCount * (FONTHEIGHT * 1.5f);
                mLineCount++;
                textItr = len;
                foundNewline = true;
                break;
            }

            if( text[c] == '\n' ){
                mFullText[ c ] = '\0';
                mTextLines[ mLineCount ].message = mFullText + textItr;
                mTextLines[ mLineCount ].offset.x = 0.0f;
                mTextLines[ mLineCount ].offset.y = mLineCount * (FONTHEIGHT * 1.5f);
                mLineCount++;
                textItr = c + 1;
                foundNewline = true;
                break;
            }
        }

        if(foundNewline){
            continue;
        }

        //Start at the end of the line and loop back until we find a space
        for(uint c = textItr + lineLen; c > textItr; c--){
            if( c >= len ){
                mTextLines[ mLineCount ].message = mFullText + textItr;
                mTextLines[ mLineCount ].offset.x = 0.0f;
                mTextLines[ mLineCount ].offset.y = mLineCount * (FONTHEIGHT * 1.5f);
                mLineCount++;
                textItr = len;
                break;
            }

            if( text[c] == ' ' ){
                if( c >= 0 ){
                    mFullText[ c ] = '\0';
                    mTextLines[ mLineCount ].message = mFullText + textItr;
                    mTextLines[ mLineCount ].offset.x = 0.0f;
                    mTextLines[ mLineCount ].offset.y = mLineCount * (FONTHEIGHT * 1.5f);
                    mLineCount++;
                    textItr = c + 1;
                    break;
                }
            }

            if( c == ( textItr + 1 ) ){
                mFullText[ c ] = '\0';
                mTextLines[ mLineCount ].message = mFullText + textItr;
                mTextLines[ mLineCount ].offset.x = 0.0f;
                mTextLines[ mLineCount ].offset.y = mLineCount * (FONTHEIGHT * 1.5f);
                mLineCount++;
                textItr += lineLen;
                break;
            }
        }
    }

    return mLineCount;
}

UIOptionBox::UIOptionBox() :
    mSelectedOption(0),
    mHighlightedOption(0),
    mScrollIndex(OPTION_MAX_OPTIONS),
    mScrollLimit(OPTION_MAX_OPTIONS),
    mNumOptions(0)
{

}

UIElement::UserChange UIOptionBox::update( bool mouseClick, XMFLOAT2 mousePosition, 
                                           bool keyPress, byte key )
{
    UserChange change;
    change.action = UserChange::Action::None;
    change.id = 0;

    if( pointCheckInside( mousePosition ) ){
        uint spot = 0;
        for(uint i = mScrollIndex - mNumOptions; i <= mScrollIndex; i++){

            float topY = ( static_cast<float>(spot) * ( FONTHEIGHT + 0.04f ) );
            float bottomY = topY + FONTHEIGHT + 0.04f;
            
            if( mousePosition.y >= topY && mousePosition.y <= bottomY ){
                if( mouseClick ){
                    mSelectedOption = i - 1;
                    mHighlightedOption = mScrollLimit + 1;
                }else{
                    mHighlightedOption = i - 1;
                }

                break;
            }

            spot++;
        }
    }else{
        mHighlightedOption = mScrollLimit + 1;
    }

    return change;
}

void UIOptionBox::addOption( char* title )
{
    assert( mNumOptions < OPTION_MAX_OPTIONS );

    strncpy( mOptions[ mNumOptions ], title, OPTION_MAX_TITLE_LEN );

    mNumOptions++;
}

char* UIOptionBox::getOption( uint index )
{
    assert( index < mNumOptions );

    return mOptions[ index ];
}

void UIOptionBox::clearOptions()
{
    mNumOptions = 0;
}

void UIOptionBox::scroll( bool up )
{
    if( up ){
        if( mScrollIndex > mNumOptions ){
            mScrollIndex--;
        }
    }else{
        if( mScrollIndex <= mScrollLimit ){
            mScrollIndex++;
        }
    }
}