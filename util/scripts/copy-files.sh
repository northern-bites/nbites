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
scp $LIB_DIR/nao/libboost_python-mt.so $LIB_DIR/nao/libprotobuf.so.7 $LIB_DIR/nao/lxv4l2.ko ../audio/* setup-robot.sh ../config/*autoload.ini $UNAME@$ADDR:
scp -r ../config/init.d $UNAME@$ADDR:/home/nao/init_stuff

echo "Now run setup-robot.sh ON THE ROBOT."
