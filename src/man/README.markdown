COMPILING & INSTALLING
======================

We use CMake for compiling and installing our code.  It has most of GNU Make's
features along with some other niceties.

How you are going to use the code greatly determines how you should configure
the build.  For most applications you will be compiling code to be run on the
robot.  Thus you want to execute the following commands to cross compile for the
robot's OS.  Do so with the following commands:

    cd src/man
    make cross

This will take you to a configuration screen.  If you want to simply test if
your code compiles, than press `c` to begin configuration and then press `g` to
generate the configuration files.

Here you should set the appropriate player for
the robot and the IP address. Help for the configuration tool is accessible by
typing h while the tool is running.

Now to compile the code simply run:

    make

If there are errors compiling and you need to change configuration files you can
delete the configuration files and make the code base ready for recompiling via:

    make clean

If you have an error compiling and need to clean your code base, but do not wish
to delete your configuration files you can delete the binaries only by using the
command:

    make cclean

Once you have built the binaries they can be installed on the robot with the
following command:

    make install


TROUBLESHOOTING:
---------------
Still having trouble?  Take a look at our [wiki](http://robocup.bowdoin.edu/trac).
