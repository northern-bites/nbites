The Northern Bites Vision System

The Northern Bites vision system has evolved into a hybrid of
run-length-encoding and scanning methods.  The main engine of
the vision system is Threshold.cpp  That file is where the main
vision loop is run and where object recognition routines are
called.  In Threshold.cpp we first threshold the entire image.
It turns out that we won't actually process many of the thresholded
pixels, but we can optimize the thresholding routine by doing
the whole image at once.

Next we check for the basic outline of the field.  We do this so
we can take advantage of knowing where the edges are in our
scanning operation.

The next step is to scan the image.  Unlike many robocup teams
we scan vertically from the bottom of the image up.  We scan
every line, but we do early stopping on the scan based on our
knowledge of where the field is and what we have seen in the
scan so far.  While we are scanning we collect linked "runs"
of the same color and feed those runs to our object recognition
data structures.  We even collect the runs for objects that are
not run-length-encoded.  The reason we do so is to provide
information on where the objects are likely to be.  Eventually
we may forgo this as well and use the field information more
directly.

Once we have made it through the scanning we call our object
recognition routines.  For any important field object, such as
posts, the ball, robots, we have a class where the processing
is done.  These are generally named in a transparent fashion.
The one current exception is ObjectFragments which is where
posts are processed.  The name is an artifact of the time where
we did run-length-encoding on all objects and processed them
all in the same place.

Each of the individual class files for the objects in turn
is linked to one or more "Visual" object files.  So, for
example, the Ball.cpp file is where balls are processed, but
in turn this file creates "VisualBall" objects which are
then used by localization, behavior, etc.  The end results is
a whole lot of vision files.  But hey, it's modular!  The current
objects we process include:

Ball.cpp - the ball
Cross.cpp - the field cross between the circle and goals
Field.cpp - the field itself
FieldLines.cpp - the lines and intersections on the field
ObjectFragments.cpp - the goals
Robots.cpp - robots (not currently working or used)

One bit of finesse: we process our lines initially before
goals in order to help ID the goals.  However, then we go
back and ID our corners after the goals in order to help
that identification process.

A common data structure used in vision processing is the
"Blob".  A Blob is more or less defined by four corners.
For now every object (except lines) uses Blobs.  Obviously
if we were to start doing serious shape processing we'd
need something more sophisticated.
