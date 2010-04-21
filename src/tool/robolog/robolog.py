
import os
import traceback
from jpype import java, JPackage, JProxy

from crobolog import *

MAGIC = 'NB_log'
LOG_EXT = '.log'

TOOL = JPackage('TOOL')


def error_wrapper(f):
    def g(*args, **kwds):
        try:
            return f(*args, **kwds)
        except Exception, e:
            traceback.print_exc()
            return java.lang.RuntimeException(str(e));
    return g


class LogHandler(object):
    '''Implements the TOOL.Data.LogHandler Java interface to check paths for
    log files and load DataSource objects for the log file or directory of log
    files.  After being wrapped by a JProxy instance, the JProxy'd LogHandler
    is passed into Java to retrieve LogSource (DataSource) objects.'''

    def __init__(self):
        pass

    @staticmethod
    def checkAvailable(dir):
        if LogHandler.check_log(dir):
            return True

        for f in os.listdir(dir):
            if LogHandler.check_log(os.path.join(dir, f)):
                return True
        else:
            return False

    @staticmethod
    def check_log(path):
        if not os.path.isfile(path):
            return False

        f = file(path, 'r')
        if f.read(6) == MAGIC:
            return True
        return False

    @staticmethod
    def getSource(path):
        if not LogHandler.checkAvailable(path):
            return None
        return LogSource(path).makeProxy()


class LogSource(object):
    '''
    Implements the TOOL.Data.DataSource interface to load C->Python->Java log
    DataSets.  Again, like the LogHandler, will be wrapped with a JProxy
    instance and returned by the LogHandler.getSource() method.  It then
    serves up requested DataSets by loading the Log files in C and creating
    Frame objects.'''
    def __init__(self, path):
        self.path = path
        self.log_files = []
        self.data_sets = []

        # Read directory for available logs
        self.addLogSource(path)

    def makeProxy(self):
        return JProxy(TOOL.Data.DataSource, inst=self)

    @error_wrapper
    def numDataSets(self):
        ''' --> Integer.  Number of log available in the directory.'''
        return len(self.log_files)

    @error_wrapper
    def getPath(self):
        ''' --> String.  Path (file URL) pointing to the directory of this
        LogSource.'''
        return self.path

    @error_wrapper
    def getType(self):
        ''' --> String.  Log source type identifier string.'''
        return TOOL.Data.DataSource.LOG_SOURCE_TYPE

    @error_wrapper
    def getDataSet(self, i):
        ''' --> TOOL.Data.DataSet.  JProxied instance of the specified
        LogDataSet.'''
        try:
            if not self.data_sets[i]:
                self.data_sets[i] = LogDataSet(self.log_files[i])

            return self.data_sets[i].makeProxy()

        except IndexError, e:
            raise java.lang.IndexOutOfBoundsException(str(e))

    @error_wrapper
    def addLogSource(self, dir):
        new_logs = set(os.path.join(dir, name)
                            for name in os.listdir(dir)
                                if os.path.splitext(name)[1] == LOG_EXT)
        new_logs.difference_update(self.log_files)
        self.log_files += filter(LogHandler.check_log, new_logs)
        self.data_sets += [None] * (len(self.log_files) - len(self.data_sets))

    @error_wrapper
    def availableLogs(self):
        jlist = java.util.Vector(len(self.log_files))
        for path in self.log_files:
            jlist.add(path)
        return jlist

    @error_wrapper
    def load(self, path):
        try:
            self.check_log(path)

            if path not in self.log_files:
                self.log_files.append(path)

            try:
                return Log.load(path)
            except Exception, e:
                print e
        except IOError, e:
            return java.io.IOException(str(e))

    @error_wrapper
    def storeLog(self, path, log):
        try:
            print dir(log)
        except IOERrror, e:
            return java.io.IOException(str(e))

    def __repr__(self):
        return '<robolog.LogSource object on directory \'%s\'>' % (
            self.path,)
    __str__ = __repr__

    @error_wrapper
    def toString(self):
        return str(self)

    @error_wrapper
    def equals(self, other):
        return hasattr(other, 'numDataSets') and hash(self) == hash(other)

    @error_wrapper
    def hashCode(self):
        return hash(self)


class LogDataSet(object):
    '''Implements the TOOL.Data.DataSet interface, since we can't extend
    TOOL.Data.AbstractDataSet in JPype.  Takes data from the C Log object and
    translates it into Frame objects.'''
    def __init__(self, path):
        self.path = str(path)

        try :
            self.log = Log.load(self.path);
        except Exception, e:
            print "Error loading C log"
            print e
            raise e

        self.data = [None] * self.log.length()
        self.loaded = False
    
    def makeProxy(self):
        return JProxy(TOOL.Data.DataSet, inst=self)

    @error_wrapper
    def hasImage(self):
        return True

    @error_wrapper
    def hasJoints(self):
        return False
    hasSensors = hasObjects = hasActions = hasComments = hasJoints

    @error_wrapper
    def robot(self):
        return self.log.dogName()

    def __str__(self):
        return "Log %s (%s,%s)" % (os.path.basename(self.path),
            self.log.dogName(), self.log.calFile())

    @error_wrapper
    def getName(self):
        return "Log " + os.path.basename(self.path)

    @error_wrapper
    def desc(self):
        return str(self)

    @error_wrapper
    def toString(self):
        return str(self)

    @error_wrapper
    def equals(self, other):
        return hasattr(other, 'frameLoaded') and hash(self) == hash(other)

    @error_wrapper
    def hashCode(self):
        return hash(self)
    
    @error_wrapper
    def size(self):
        return self.log.length()

    @error_wrapper
    def loaded(self):
        return self.loaded
    @error_wrapper
    def previewsLoaded(self):
        return self.loaded

    @error_wrapper
    def frameLoaded(self, i):
        return self.loaded or self.data[i] is not None
    
    @error_wrapper
    def load(self):
        for i in xrange(len(self.data)):
            self.loadFrame(i)
    @error_wrapper
    def loadPreviews(self):
        self.load()

    @error_wrapper
    def loadFrame(self, i):
        try:
            if not 0 <= i < self.size():
                return java.lang.IndexOutOfBoundsException('Index: %i, Size: %i'
                    % (i, self.size()))
    
            if self.data[i] is not None:
                return
    
            image = self.log.robot(i).image()
            if image is None:
                return java.lang.IndexOutOfBoundsException('Index: %i, Size: %i'
                    % (i, self.size()))
    
            self.data[i] = TOOL.Data.Frame(self.makeProxy(), i)
    
            s = '\0' * (image.width() * image.height() * 3)
            image.storeRaw(s)
            ycbcr = TOOL.Image.YCbCrImage(s, image.width(), image.height())
            self.data[i].setImage(ycbcr)
        except Exception, e:
            print e
            raise e

        image = self.log.robot(i).image()
        self.data[i] = TOOL.Data.Frame(self.makeProxy(), i)

        s = '\0' * (image.width() * image.height() * 3)
        image.storeRaw(s)
        ycbcr = TOOL.Image.YCbCrImage(s, image.width(), image.height())
        self.data[i].setImage(ycbcr)

    @error_wrapper
    def getFrame(self, i):
        if not self.frameLoaded(i):
            self.loadFrame(i)
        return self.data[i]

    @error_wrapper
    def getPreview(self, i):
        return self.getFrame(i).getPreview()

    @error_wrapper
    def storeFrame(self, f):
        pass


