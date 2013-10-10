#include "EventManager.h"

EventManager* EventManager::mEventManager;

EventManager* EventManager::getEventManager()
{
    if( !mEventManager ){
        mEventManager = new EventManager();
    }

    return mEventManager;
}

void EventManager::destroyEventManager()
{
    if( mEventManager ){
        delete mEventManager;
    }
}

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
    mNumHandlers++;
    return true;
}

bool EventManager::queueEvent( Event& e )
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