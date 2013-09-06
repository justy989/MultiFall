#include "UserBinds.h"

#include <assert.h>

UserBinds::UserBinds()
{

}

void UserBinds::updateKeyStates( RAWINPUT* rawInput )
{
    if(rawInput->header.dwType== RIM_TYPEMOUSE){
        bool leftButtonDown = rawInput->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN;
        bool rightButtonDown = rawInput->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN;

        for(int i = 0; i < USERBIND_ACTION_COUNT; i++){
            if( VK_LBUTTON == mBinds[ i ].key ){
                mBinds[ i ].isDown = leftButtonDown;
            }

            if( VK_RBUTTON == mBinds[ i ].key ){
                mBinds[ i ].isDown = rightButtonDown;
            }
        }

    }else if (rawInput->header.dwType== RIM_TYPEKEYBOARD){
        USHORT VKey = rawInput->data.keyboard.VKey;
        bool keyUp = rawInput->data.keyboard.Flags & RI_KEY_BREAK; 

        for(int i = 0; i < USERBIND_ACTION_COUNT; i++){
            if( VKey == mBinds[ i ].key ){
                mBinds[ i ].isDown = !keyUp;
                break;
            }
        }
    }
}

void UserBinds::bindKey( Action action, char key )
{
    assert( action < USERBIND_ACTION_COUNT );
    mBinds[ action ].key = key;
}

bool UserBinds::isBindDown( Action action )
{
    assert( action < USERBIND_ACTION_COUNT );
    return mBinds[ action ].isDown;
}