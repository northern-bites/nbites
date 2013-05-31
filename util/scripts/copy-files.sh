# Copies important files to a newly-flashed nao's home folder.

if [ $# -ne 2 ]; then
    echo "Usage: ./copy-files <robot-address> <username>"
    echo "Try robot-address = nao.local and username = nao if unsure."
    exit 1
fi

ROBOT=$1
ROBOT_UNAME=$2
ROBOCUP=robocup.bowdoin.edu:/mnt/research/robocup
FOLDER=nao_files

# Get the files from the server
echo "Type username for RoboCup server, followed by [ENTER]"
read SERVER_UNAME

rsync -vr $SERVER_UNAME@$ROBOCUP/$FOLDER .

# Copy important libraries to home folder
echo "Copying necessary files to the robot."
scp -r $FOLDER setup-robot.sh $ROBOT_UNAME@$ROBOT:

echo "Now run setup-robot.sh ON THE ROBOT."
