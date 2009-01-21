

import errno
import socket
import sys
import traceback

from ..util import FSA

class SoccerPlayer(FSA.FSA):
    PORT = 4010
    BUFFER_SIZE = 2048
    def __init__(self, brain):
        FSA.FSA.__init__(self,brain)
        self.brain = brain

        self.addState('bind', self.bind)
        self.addState('accept', self.accept)
        self.addState('listen', self.listen)
        self.addState('compile', self.compile)
        self.addState('execute', self.execute)
        self.addState('respond', self.respond)
        self.addState('subplayer', self.subplayer)
        self.addState('error', self.error)
        self.addState('close', self.close)
        self.addState('done', self.done)
        self.addState('gameInitial', self.gameInitial)
        self.addState('gameReady', self.gameReady)
        self.addState('gameSet', self.gameSet)
        self.addState('gamePlaying', self.gamePlaying)
        self.addState('gameFinished', self.gameFinished)

        self.currentState = 'listen'
        self.setName('Player pRexec')
        self.setPrintStateChanges(False)
        self.setPrintFunction(self.brain.out.printf)

        self.bind_socket = None
        self.socket = None
        self.rhost = None
        self.rport = None
        self.msg = ''
        self.code = None
        self.repeat = False
        self.sub_player = None

        self.connected = False
        self.bind_failure = False
        self.exc_info = None
        self.exc_limit = None
        self.exc_output = sys.stdout

    @staticmethod
    def bind(self):
        if not self.socket:
            try:
                self.bind_socket = socket.socket()
                self.bind_socket.bind(('', self.PORT))
                self.bind_socket.setblocking(False)
                self.bind_socket.listen(5)
            except Exception, e:
                self.exc_info = sys.exc_info()
                self.bind_failure = True
                return self.goNow('error')
    
        if self.connected:
            return self.goNow('listen')
        else:
            return self.goNow('accept')

    @staticmethod
    def accept(self):
        if not self.connected:
            if not self.bind_socket:
                return self.goNow('bind')
            try:
                self.socket, (self.rhost,
                              self.rport) = self.bind_socket.accept()
                self.socket.setblocking(False)
            except socket.error, e:
                if e.args[0] == errno.EAGAIN:
                    return self.stay()
                else:
                    self.exc_info = sys.exc_info()
                    self.bind_failure = True
                    return self.goNow('error')

        return self.goNow('listen')

    @staticmethod
    def listen(self):
        try:
            self.msg = self.socket.recv(self.BUFFER_SIZE)
        except socket.error, e:
            if e.args[0] != errno.EAGAIN:
                self.exc_info = sys.exc_info()
                return self.goNow('error')
            elif self.sub_player:
                return self.goNow('subplayer')
            elif self.code and self.repeat:
                return self.goNow('execute')
            else:
                return self.stay()

        if not self.msg:
            # socket closed
            return self.goNow('close')
        else: 
            # received command, respond
            return self.goNow('respond')

    @staticmethod
    def compile(self):
        if self.msg:
            if self.rhost:
                source = '<' + self.rhost + ':' + str(self.rport) + '>'
            else:
                source = '<unknown>'

            try:
                self.code = compile(self.msg, source, 'single')
                self.msg = ''
            except Exception, e:
                self.exc_info = sys.exc_info()
                return self.goNow('error')
            else:
                return self.goNow('execute')
        return self.goNow('listen')

    @staticmethod
    def execute(self):
        if self.code:
            try:
                exec self.code in globals(), self.__dict__
            except Exception, e:
                self.exc_info = sys.exc_info()
                self.code = None
                return self.goNow('error')

        if not self.repeat:
            self.code = None
        return self.goLater('listen')

    @staticmethod
    def respond(self):
        try:
            self.socket.send('ACK\n')
        except socket.error, e:
            self.exc_info = sys.exc_info()
            return self.goNow('error')

        return self.goNow('compile')

    @staticmethod
    def subplayer(self):
        if self.child:
            self.child.run()
            return self.goLater('listen')
        return self.goNow('listen')

    @staticmethod
    def error(self):
        if self.exc_info:
            traceback.print_exception(etype=self.exc_info[0],
                                      value=self.exc_info[1],
                                      tb=self.exc_info[2],
                                      limit=self.exc_limit,
                                      file=self.exc_output)
            if self.bind_failure:
                return self.goNow('done')
            elif self.exc_info[0] is socket.error:
                return self.goNow('close')
        return self.goNow('listen')

    @staticmethod
    def close(self):
        if self.socket:
            self.socket.close()
            self.socket = None
        self.connected = False
        return self.goNow('accept')

    @staticmethod
    def done(self):
        if self.socket:
            self.socket.close()
        if self.bind_socket:
            self.bind_socket.close()
        self.socket = None
        self.bind_socket = None
        self.connected = False
        return self.stay()
   
    @staticmethod
    def gameInitial(self):
        return self.goNow('bind')
    gameReady = gameSet = gamePlaying = gameInitial

    @staticmethod
    def gameFinished(self):
        return self.goNow('done')

    def __del__(self):
        if self.socket:
            self.socket.close()
        if self.bind_socket:
            self.bind_socket.close()
