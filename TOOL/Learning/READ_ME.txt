Our learning system.  For now this module is responsible for the maintenance of
KEY.KEY files associated with any given frame directory.  The KEY.KEY files use
Google's Protocol Buffer system for serialized data.  See:

http://code.google.com/apis/protocolbuffers/docs/overview.html

for more details.  The protocol buffer itself is setup in TOOL/Data/Classify.proto
and is used to generate TOOL/Data/Classification.java automatically.  That
file is where all of the code to interface with the file lives.

When we are in the learning system we listen for data events.  There are two kinds that
we care about:  when a new data set is selected and when a new frame is selected.

When a new data set is selected we look in the set's directory for a file called
"KEY.KEY".  If the file exists then we read it into our data structure (provided
by Classification.java).  If it doesn't then we fill our data structure
in with default records.

When a new frame is called we check our data structure.  One of the fields in the
file specifies whether a human has approved the frame contents or not.  If that
field is set to true, then we use the data in the data structure to set the
GUI to reflect the contents of the data structure.  A human can still edit the
contents if they want.  If that field is set to false, then we instead default
to using the vision system to set the basic contents of the frame.  This will
hopefully allow for faster data entry when we are editting.

Another way to speed editting is the "Use Last" button which will fill in the data
from the last frame.  This is often useful because in many sets there isn't much
change in the data from frame to frame.

Once you are done editting you can hit the "Write" button and it will write the
contents of the data structure out to the KEY.KEY file.  Note that this is
the only way to write out this information.

We can also run batch jobs now.  Either on a single set of data or on an entire
master directory.  When running batch jobs you can set up logging information to
print by clicking on the appropriate check boxes (e.g. if you want all the files
listed with false balls then click the "false balls" check box).  When running
in batch the system will check for the appropriate key files.  It will only process
frames for which there is a key file and and also that the key file has human
editting set for that particular frame.
