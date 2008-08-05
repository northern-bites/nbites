
try:
    import _leds
except ImportError:
    import sys
    print >>sys.stderr, "**** WARNING - no backend _leds module located ****"
