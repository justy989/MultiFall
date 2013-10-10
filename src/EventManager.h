#ifndef APP_EVENTMANAGER_H
#define APP_EVENTMANAGER_H

#include "Event.h"

#include <queue>

//Doesn't need to be that high >.>
#define EVENTMANAGER_MAX_HANDLERS 16
#define EVENTMANAGER_MAX_QUEUE_EVENTS 1024

#define EVENTMANAGER EventManager::getEventManager() 

class EventManager;

class EventHandler{
public:

    EventHandler() {}

    //Called by the event manager when events need to be handled
    virtual void handleEvent( Event& e ) = 0;

protected:

};

class EventManager{
public:

    static EventManager* getEventManager();
    static void destroyEventManager();

    //Registers a handler to start recieving events
    bool registerHandler( EventHandler* handler );

    //Pushes back an event to be processed 
    bool queueEvent( Event& e );

    //Will process events until the queue is empty
    void process( );

protected:

    EventManager();

    static EventManager* mEventManager;

    //List of handlers
    EventHandler* mHandlers[ EVENTMANAGER_MAX_HANDLERS ];
    uint mNumHandlers;

    //Queue of events
    std::queue< Event > mEventQueue;
};

#endif