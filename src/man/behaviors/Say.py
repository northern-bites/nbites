"""
Say.py
A wrapper for the "say" system command which PKoch discovered in June 2016
"""

import os
import subprocess

class Say:
	IN_DEBUG, IN_SCRIMMAGE, IN_GAME = range(3)

	# @TODO: We should only declare the say debug level once; instead, we're
	# declaring it here and in src/share/include/TextToSpeech.h. We should
	# find a way to consolidate these declarations since they're basically
	# copies of each other.
	when = IN_SCRIMMAGE

def say(upTo, sayThis):
	"""
	Usage: 
	from ..Say import *
	say(Say.IN_DEBUG, "Hello world")
	"""
	if len(sayThis) > 140:
		print "Stop trying to say super long things. Max 140 chars."
		return
	if upTo < Say.when:
		print "Say was blocked: " + sayThis
		return
	# If we get here we should be allowed to say stuff
	if os.fork() == 0:
		print "Say: " + sayThis
		subprocess.call("say \"" + sayThis + "\"", shell=True)
		os._exit(0)
