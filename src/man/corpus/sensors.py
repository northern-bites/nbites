
import sys
try:
    import _sensors
except ImportError:
    print >>sys.stderr,"**** WARNING - no backend _sensors module located ****"
    _sensors = None

if _sensors and not hasattr(_sensors, "inst"):
    raise ImportError("C++ _sensors backend is not initialized.")


def Sensors():
    return _sensors.inst
