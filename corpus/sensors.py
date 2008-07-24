
import sys
import _sensors

if not hasattr(_sensors, "inst"):
    raise ImportError("C++ _sensors backend is not initialized.  Did you mean "
                      "to compile with USE_PYSENSORS_FAKE_BACKEND?")

#class Sensors(_sensors.Sensors):
#    def __init__(self):
#        _sensors.Sensors.__init__(self, _sensors.inst)


def Sensors():
    return _sensors.inst
