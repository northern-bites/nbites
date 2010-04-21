
__name__ = 'synchro'

class Event(object):
    '''
    An abstract Event, recieved from a Synchronizer.

    Used to pass and hold reference to a specific event.  A string name is
    associated with the event, but the object itself is used in the backend
    map.
    '''
    name = 'the name of this event'

    def await():
        '''
        Wait, deferring processing time to other threads, until the
        given event is signalled.

        If the event has been signalled previously and hast not yet
        been caught, this method will return immediately and clear the
        signal.
        '''

    def poll():
        '''
        Return a boolean indicating whether the given event has
        occurred.

        Returns True only if the event has been signalled after the
        last call to poll() or await().
        '''

    def signal():
        '''
        Signal occurance of the event.

        If any threads are waiting for the Event, a single thread will
        resume from wait, immediately clearing the signal.  If no threads
        await, the signal will persist until the Event is polled or
        awaited.
        '''


class Synchro(object):
    '''
    A generic Synchronizer object.

    Maintains a collection of Events (mutexs), which may be polled or
    awaited.
    '''

    def available():
        '''Returns a list of events available for synchronization.'''

    def await(event):
        '''
        Wait, deferring processing time to other threads, until the
        given event is signalled.

        If the event has been signalled previously and hast not yet
        been caught, this method will return immediately and clear the
        signal.
        '''

    def create(name):
        '''
        Register a new event with the synchronizer.

        Returns an Event object, which is also retrievable via available()
        and the Event's name.  The event object itself should be passed to
        any of the Synchronizer methods that act on Events, not the event's
        name.
        '''

    def poll(event):
        '''
        Return a boolean indicating whether the given event has
        occurred.

        Returns True only if the event has been signalled after the
        last call to poll() or await().
        '''

    def signal(event):
        '''
        Signal occurance of the event.

        If any threads are waiting for the Event, a single thread will
        resume from wait, immediately clearing the signal.  If no threads
        await, the signal will persist until the Event is polled or
        awaited.
        '''

