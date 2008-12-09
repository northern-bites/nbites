INSTALLING LIBRARIES AND CROSS COMPILER

In order to compile the man binaries, you must first have the NaoQI libraries  and associated cross compiler installed on your machine.  The archives needed to do this are available on our server. The default configuration is to install to /usr/local/nao.

NOTE: The given commands should be executed from the directory where you downloaded the archives. Also, depending on your system setup, you may need root privilages to install to /usr/local/nao

First, get the NaoQI libraries by saving the file
<https://robocup.bowdoin.edu/files/software/nao/NaoQi/NaoQi-1.0.0-Linux.tar.gz> to your computer. You should then extract the contents to /usr/local/nao (i.e. there should be a file /usr/local/nao/Release-notes.txt)

	sudo tar -xvzf NaoQiRobocup-1.0.0-Linux -C /usr/local
	sudo mv /usr/local/NaoQiRobocup-1.0.0-Linux /usr/local/nao

You will also need the cross compiler, so start by creating its directory:

	sudo mkdir /usr/local/nao/crosstoolchain

Next,if you are on linux, the new (1.0) cross tool chain is available from <https://robocup.bowdoin.edu/files/software/nao/cross_compiler_stuff/ctc-1.0.0b.tar.bz2>
If you are on a mac, there is no precompiled cross compiler (yet). You might try using the stagging folder from the Linux file above, combined with the cross folder from the old cross compiler for mac at <https://robocup.bowdoin.edu/files/software/nao/cross_compiler_stuff/cross-mac-intel.tar.bz2>

When extracted, this should give you a cross folder that will replace the empty cross folder in /usr/local/crosstoolchain. Once you have downloaded the cross-mac-intel.tar.bz2, extract it to the crosstoolchain folder with the command:

     	sudo tar --strip 1 -C /usr/local/nao/crosstoolchain -xvjf ctc-1.0.0b.tar.bz2

Double check to make sure you have folders named 'cross' and 'stagging' in $AL_DIR/crosstoolchain

COMPILING & INSTALLING

We use CMake for compiling and installing our code.  It has most of GNU Make's
features along with some other niceties.

How you are going to use the code greatly determines how you should configure
the build.  For most applications you will be compiling code to be run on the
robot.  Thus you want to execute the following commands to cross compile for the
robot's OS.  Do so with the following commands (where $ROBOCUP_HOME is the path
to your robocup directories):

   cd $ROBOCUP_HOME/man
   make cross

This will take you to a configuration screen.  If you want to simply test if
your code compiles, than press c to begin configuration and then press g to
generate the configuration files.

If you are compiling for the robot you should make sure @MAN_IS_REMOTE@ is set
to ON.  If it is set to OFF, you can change it by moving over the OFF text area
and hitting the enter key.  Here you should also set the appropriate player for
the robot and the IP address. Help for the configuration tool is accesible by
typing h while the tool is running.

Now to compile the code simply run:

    make

If there are errors compiling and you need to change configuration files you can
delete the configuration files and make the codebase ready for recompiling via:

   make clean

If you have an error compiling and need to clean your codebase, but do not wish
to delete your configuration files you can delete the binaries only by using the
command:

    make cclean

Once you have built the binaries they can be installed on the robot with the
following command:

    make install
