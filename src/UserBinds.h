#ifndef USER_BINDS_H
#define USER_BINDS_H

#include <Windows.h>

#define USERBIND_ACTION_COUNT 14

class UserBinds{
public:

    UserBinds();

    void updateKeyStates( RAWINPUT* rawInput );

    enum Action{
        //Player Actions
        Player_Move_Forward, //Move forward
        Player_Move_Backward, //Move back
        Player_Move_Left, //strafe left
        Player_Move_Right, //Strafe right
        Player_Jump, //jump upwardly
        Player_Crouch, //crouch down
        Player_Attack, //Attack using the current weapon
        Player_Cast, //Cast the current spell
        Player_Activate, //active what the user is looking at

        //Menu controls
        Menu_Pause, //Pause the game
        Menu_Select, //Select an option
        Menu_Cancel, //Cancel/Go back
        Menu_Cycle_Forward, //Select the next option
        Menu_Cycle_Back //Select the previous option
    };

    struct Bind{
        char key;
        bool isDown;
    };

    void bindKey( Action action, char key );
    bool isBindDown( Action action );

protected:

    Bind mBinds[ USERBIND_ACTION_COUNT ];
};

#endif