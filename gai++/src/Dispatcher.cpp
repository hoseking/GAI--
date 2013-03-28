
#include "Dispatcher.h"

#include <exception>
#include <pthread.h>

#include "DataStore.h"
#include "GAI.h"


namespace GAI
{
	
	Dispatcher::Dispatcher( DataStore& data_store, bool opt_out, double dispatch_interval ) :
	mbOptOut( opt_out ),
	mDispatchInterval( dispatch_interval ),
    mDataStore( data_store ),
	mDispatchEvent( NULL ),
	mDispatchEventBase( NULL )
	{
		createTimerThread();
	}
	
	Dispatcher::~Dispatcher()
	{
		event_del( mDispatchEvent );
		pthread_join( mTimerThread, NULL );
	}
    
    bool Dispatcher::storeHit( const Hit& hit )
    {
        // really: add this to the datastore
        throw "Not yet implemented";
    }
	
	void Dispatcher::queueDispatch()
    /// really: send outstanding hits if possible
	{
		throw "Not yet implemented";
	}
	
	void Dispatcher::cancelDispatch()
	{
		throw "Not yet implemented";
	}
	
	bool Dispatcher::isOptOut() const
	{
		return mbOptOut;
	}
	
	void Dispatcher::setOptOut( const bool opt_out )
	{
		mbOptOut = opt_out;
	}
	
	int Dispatcher::getDispatchInterval() const
	{
		return mDispatchInterval;
	}
	
	void Dispatcher::setDispatchInterval( const int dispatch_interval )
	{
		mDispatchInterval = dispatch_interval;
		
		if( mDispatchEvent && event_initialized( mDispatchEvent ) )
		{
			const struct timeval timeout = {mDispatchInterval, 0};
			event_add( mDispatchEvent, &timeout );
		}
	}
	
	void Dispatcher::createTimerThread()
	{
		pthread_create( &mTimerThread, NULL, &Dispatcher::timerThread, this );
	}
	
	void* Dispatcher::timerThread( void *context )
	{
		Dispatcher *dispatcher = static_cast<Dispatcher*>( context );
		
        dispatcher->mDispatchEventBase =  event_base_new();
        dispatcher->mDispatchEvent = event_new( dispatcher->mDispatchEventBase, -1, EV_TIMEOUT|EV_PERSIST, Dispatcher::timerCallback, context );
		
		const struct timeval timeout = {dispatcher->mDispatchInterval, 0};
        event_add( dispatcher->mDispatchEvent, &timeout );
		
        event_base_dispatch( dispatcher->mDispatchEventBase );
		
		event_free( dispatcher->mDispatchEvent );
		event_base_free( dispatcher->mDispatchEventBase );
	}
	
	void Dispatcher::timerCallback( evutil_socket_t file_descriptor, short events, void* context )
	{
		Dispatcher *dispatcher = static_cast<Dispatcher*>( context );
		
		dispatcher->queueDispatch();
	}
	
}
