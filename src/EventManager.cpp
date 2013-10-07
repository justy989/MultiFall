#include "EventManager.h"


EventManager::EventManager() :
    mNumHandlers(0)
{

}

bool EventManager::registerHandler( EventHandler* handler )
{
    if( mNumHandlers >= EVENTMANAGER_MAX_HANDLERS ){
        return false;
    }

    mHandlers[ mNumHandlers ] = handler;
    mHandlers[ mNumHandlers ]->setEventManager( this );
    mNumHandlers++;
    return true;
}

bool EventManager::enqueueEvent( Event& e )
{
    if( mEventQueue.size() >= EVENTMANAGER_MAX_QUEUE_EVENTS ){
        return false;
    }

    mEventQueue.push( e );
    return true;
}

void EventManager::process( )
{
    while( mEventQueue.size() ){
        for(int i = 0; i < mNumHandlers; i++){
            mHandlers[i]->handleEvent( mEventQueue.front() );
        }

        mEventQueue.pop();
    }
}