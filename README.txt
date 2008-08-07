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
order to install to /usr/local.  If you don

The script will prompt you for a username, you should enter your Bowdoin LDAP
username, you will then be asked to enter your password, in which case you
should enter the associated password.

The script will take a while to download everthing and compile. If all goes
correctly you should see no error messages and be greated with a nice "Done!"


COMPILING THE CODE

We compile via cmake. To compile the code simply run make in the man directory.

   cd $ROBOCUP_HOME/man
   make

If it is your first time running you will be taken to a configuration screen.
To simply generate the files and continue compiling type g.
If you need to configure anything the help commands at the bottom are
informative.

INSTALLING THE SOURCE
