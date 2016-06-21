# Copies important files to a newly-flashed nao's home folder.

if [ $# -ne 2 ]; then
    echo "Usage: ./copy-files <robot-address> <username>"
    echo "Try robot-address = nao.local and username = nao if unsure."
    exit 1
fi

ROBOT=$1
ROBOT_UNAME=$2
ROBOCUP=dover:/mnt/research/robocup
FOLDER=nao_files
WIFI_CONFIG=wireless_config

# Get the files from the server
echo "Downloading files FROM ROBOCUP!"
echo "Type username for RoboCup server, followed by [ENTER]"
read SERVER_UNAME

rsync -vr $SERVER_UNAME@$ROBOCUP/$FOLDER .

# Must remove previous known_host for nao.local otherwise scp 
# will fail with alert that SOMEONE IS DOING SOMETHING NASTY
ssh-keygen -f "/home/$(whoami)/.ssh/known_hosts" -R nao.local

# Copy important libraries to home folder
echo "Copying necessary files TO THE ROBOT!"
scp -r $FOLDER $WIFI_CONFIG tts_autoload.ini setup-robot.sh $ROBOT_UNAME@$ROBOT:

echo "Now run setup-robot.sh ON THE ROBOT."
