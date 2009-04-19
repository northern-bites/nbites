#Makefile to enable syncing the color table collection with the server.
#Alternatively, you can sync manually.  In the future, once there are many tables
#it would probably make sense to make targets for specific to competition, lab,etc
#More detailed documentation is available at
# http://robocup.bowdoin.edu/trac/wiki/ColorTables

#The default is to read your username from the shell.
#USERNAME = $(shell echo $$USER)
#USERNAME = <enter your bowdoin id here>
USERNAME = robocup
EXCLUDE = Makefile*
SOURCE = robocup.bowdoin.edu:/home/robocup/frames
DEST = .
RSYNC = rsync
RSYNC_OPTS = -rvzc  #recursive, verbose, compress, checksum (binary diff)

pull:
	$(RSYNC) $(RSYNC_OPTS) --exclude='$(EXCLUDE)'\
	 $(USERNAME)@$(SOURCE)/* $(DEST)

push:
	$(RSYNC) $(RSYNC_OPTS) --exclude='$(EXCLUDE)'\
	  $(DEST)/* $(USERNAME)@$(SOURCE)

diff:
	$(RSYNC) $(RSYNC_OPTS) -n --exclude='$(EXCLUDE)'\
		  $(DEST)/* $(USERNAME)@$(SOURCE)
