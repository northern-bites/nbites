#This script scp's the current PSO kick to the robot

if [ $# -ne 2 ]; then
    echo "Usage: ./scpKick <robot-address> <username>"
    echo "Try robot-address = robot name and username = nao if unsure."
    exit 1
fi
echo "Start installing to robot"

ROBOT=$1
ROBOT_UNAME=$2
FILE=PSOMoves.py
echo "Make sure that make cross is configured already"
sed -i 's/^/py/' $FILE

cd ~/nbites/src/man
make cross

make install
echo "Installing man to $ROBOT"

ssh $ROBOT_UNAME@$ROBOT "
    man restart
    "