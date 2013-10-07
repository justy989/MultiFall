#ifndef APP_EVENTMANAGER_H
#define APP_EVENTMANAGER_H

#include "Event.h"

#include <queue>

//Doesn't need to be that high >.>
#define EVENTMANAGER_MAX_HANDLERS 16
#define EVENTMANAGER_MAX_QUEUE_EVENTS 1024

class EventManager;

class EventHandler{
public:

    EventHandler() : mEventManager(NULL) {}

    //Called by the event manager when events need to be handled
    virtual void handleEvent( Event& e ) = 0;

protected:

    //Just to call setEventManager
    friend class EventManager;

    inline void setEventManager( EventManager* eventManager );

    EventManager* mEventManager;
};

inline void EventHandler::setEventManager( EventManager* eventManager ){mEventManager = eventManager;}

class EventManager{
public:

    EventManager();

    //Registers a handler to start recieving events
    bool registerHandler( EventHandler* handler );

    //Pushes back an event to be processed 
    bool enqueueEvent( Event& e );

    //Will process events until the queue is empty
    void process( );

protected:

    //List of handlers
    EventHandler* mHandlers[ EVENTMANAGER_MAX_HANDLERS ];
    uint mNumHandlers;

    //Queue of events
    std::queue< Event > mEventQueue;
};

#endif