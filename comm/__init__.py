
try:
    from _comm import *
except ImportError:
    import sys
    print >>sys.stderr, "**** WARNING - No backend _comm module located ****"
    inst = None
    
