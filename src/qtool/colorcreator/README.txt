Color table builder based on a design by Bill Silver.  The tool does several
things.

1) Reads and displays Northern Bites Frames.  Can display a frame in many formats:
    Straight - e.g. like a camera
    Y        - Just Y channel information
    U        - Just U channel information
    V        - Just V channel information
    R        - Just R channel information
    G        - Just G channel information
    B        - Just B channel information
    H        - Just H channel information
    S        - Just S channel information
    V        - Just V channel information
    Edge     - Edges in the YUV space (jump from pixel to pixel of more than X)
    Threshold - Displays the image using either a color table, or slider info

2) Reads color tables of two formats.  Our old format has tons of colors and a
unique number for each. We are switching to a bit format where each main color
has a bit assigned to it.  We can only store 9 colors this way (including
undefined), but it allows us to do some more interesting things with color.

3) Builds color tables.  This is the most important piece of functionality.  The
idea is pretty simply.  There are sliders to set various minimum and maximum
values in the color space (e.g. Y).  For each color these define a region of
the color space associated with that color.  The beauty of this system is that
we can very quickly build a color table and it won't have all of the weird
holes and things that our old system had.  The downside is that we do not have
the fine grained control of the old system.  Of course one could build a table
with this system and then refine it with the old if they were so inclined.

4) Converts old color tables into the new format.  Obviously in the long run
this functionality will be dropped.

edit 10/4/11 (Lizzie) The color creator is now broken down into several widgets.
The main widget includes the three different views of robot images. There are also
three dock widgets that contain buttons, the color editor, and the view options.
Each of these widgets' size can be increased, and they can also be "popped out"
into their own windows. 

Also, the deprecated go to file button was removed, and initial loading of a file
was improved so that images load immediately instead of after clicking "next" for
the first time. 
