
import sys

try:
    from aibovision import v as aibo
except ImportError, e:
    print >>sys.stderr, "No aibovision extension loaded"
    print >>sys.stderr, e


try:
    from naovision import v as nao
except ImportError, e:
    print >>sys.stderr, "No naovision extension loaded"
    print >>sys.stderr, e

