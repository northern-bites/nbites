# Copies important files to a newly-flashed nao's home folder.

if [ $# -ne 2 ]; then
    echo "Usage: ./copy-files <robot-address> <username>"
    echo "Try robot-address = nao.local and username = nao if unsure."
    exit 1
fi

ADDR=$1
UNAME=$2

LIB_DIR=../../lib

# Copy important libraries to home folder
echo "Copying necessary files to the robot."
scp -r $LIB_DIR/nao/* ../audio/* setup-robot.sh ../config/* $UNAME@$ADDR:

echo "Now run setup-robot.sh ON THE ROBOT."
