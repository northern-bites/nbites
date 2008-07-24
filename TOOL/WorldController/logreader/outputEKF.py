#!/usr/bin/env python

__version__ = "$Revision: 1.0 $"

# outputEKF -- script to build a world controller readable log file from either
# nao of aibo log files of the type locLog

import sys
#import naoEKFOut.naoEKFOut
import aiboEKFOut.aiboEKFOut

INFO_LENGTH = 3
LOG_TYPE_INDEX = 2
NAO_TYPE = "NAO"

def main(*args):
    # simple input
    if len(args) != 1:
	print "USAGE: python outputEKF [FILE]"
	return

    # reads from input file
    infile = file(args[0], 'r')
    print "infile: ", args[0]

    # reads the first line and determine if we are reading a nao or aibo log
    basicInfo = infile.readline().split(" ")

#     if (len(basicInfo) > 2 and basicInfo[LOG_TYPE_INDEX] == NAO_TYPE):
#         print "USING NAO"
#         infile.close()
#         naoEKFOut.naoEKFOut.main(args[0])
#     else:
    print "USING AIBO"
    infile.close()
    aiboEKFOut.aiboEKFOut.main(args[0])
    
if __name__=='__main__':
    sys.exit(main(*sys.argv[1:]))
