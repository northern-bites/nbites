
import sys
try:
    import _sensors
except ImportError:
    print >>sys.stderr,"**** WARNING - no backend _sensors module located ****"
    _sensors = None

if _sensors and not hasattr(_sensors, "inst"):
    raise ImportError("C++ _sensors backend is not initialized.  Did you mean "
                      "to compile with USE_PYSENSORS_FAKE_BACKEND?")


def Sensors():
    return _sensors.inst
