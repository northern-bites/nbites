import sys
try:
    import vision
except ImportError:
    print >>sys.stderr, "**** WARNING: no backend vision module located ****"

    vision = None

if vision and not hasattr(vision, "inst"):
    raise ImportError("C++ vision backend is not initialized. Did you mean to compile with USE_PYVISION_FAKE_BACKEND?")


def Vision():
    return vision.inst
