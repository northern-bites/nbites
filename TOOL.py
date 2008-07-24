
import jpype
import platform
import threading
import time
import code
import sys
try:
    import robolog
except ImportError:
    pass

MAX_HEAP_SIZE = 512

DARWIN = 'Darwin'
PLATFORM = platform.system()
if PLATFORM == DARWIN:
    try:
        from PyObjCTools import AppHelper
    except ImportError:
        print "Running on a Mac, you need PyObjC"
        print "Please install this package from sourceforge"
CLASSPATH = 'TOOL.jar'

global TOOL
global tool

def println(s):
    sys.stdout.write(s + '\n')
    sys.stdout.flush()

def startTOOL():
    jpype.startJVM(jpype.getDefaultJVMPath(),
        '-cp "%s" -Xmx %i' % (CLASSPATH, MAX_HEAP_SIZE))

    if PLATFORM == DARWIN:
        th = threading.Thread()
        th.run = runTOOL
        th.start()

        AppHelper.runConsoleLoop()
    else:
        runTOOL()

def runTOOL():
    global TOOL, tool
    TOOL = jpype.JPackage('TOOL')
    init_logging()
    tool = TOOL.TOOL()
    while tool:
        time.sleep(10)


def init_logging():
    global TOOL
    log_handler = robolog.LogHandler()
    proxy = jpype.JProxy('TOOL.Data.LogHandler', inst=log_handler)
    TOOL.Data.SourceManager.DEFAULT_LOG_HANDLER = proxy;

if __name__ == '__main__':
    if len(sys.argv) > 1:
	if sys.argv[1] == '-p':
	    jpype.startJVM(jpype.getDefaultJVMPath(), *sys.argv[2:])
    startTOOL()
