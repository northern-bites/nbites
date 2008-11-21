

class Event(object):
    name = None


class Synchro(object):
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

    def poll(event):
        '''
        Return a boolean indicating whether the given event has
        occurred.

        Returns True only if the event has been signalled after the
        last call to poll() or await().
        '''

    def create(name):
        '''Register a new event with the synchronizer.'''

