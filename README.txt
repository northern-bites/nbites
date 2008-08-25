INSTALLING LIBRARIES AND CROSS COMPILER

In order to compile the man binaries, you must first have the NaoQI libraries
and associated cross compiler installed on your machine.  To this purpose we
have a nice script available on our server
<http://robocup.bowdoin.edu/files/software/nao/build_script.sh>.

You will first have to make the script runable.  One way to do this, is by
running the following command in the directory, where you downloaded the
script.

	chmod 755 build_script.sh

The script will download the appropriate files from the nBites server, build the
libraries, and install them on your computer.  The default install location is
/usr/local/nao.  You can change this location by editing AL_DIR at the top of
the file, however you will have to define an enivornment variable $AL_DIR in
your shell.  To do this in bash add the following to your config file

     AL_DIR="/path/to/install"
     export AL_DIR

NOTE: Depending on how your system is setup, you may need root permissions in
order to install to /usr/local.

The script will prompt you for a username, you should enter your Bowdoin LDAP
username, you will then be asked to enter your password, in which case you
should enter the associated password.

The script will take a while to download everthing and compile. If all goes
correctly you should see no error messages and be greated with a nice "Done!"


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
