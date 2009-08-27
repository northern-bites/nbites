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

Once you are done editting you can hit the "Write" button and it will write the
contents of the data structure out to the KEY.KEY file.  Note that this is
the only way to write out this information.

In the (near) future we will extend this module with the fun stuff - the ability
to run lots of data sets at once and collect up the results for machine learning
or for human use.
