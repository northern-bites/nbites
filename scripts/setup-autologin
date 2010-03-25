#!/bin/sh
###############################################################################
# This script will setup auto-login into a remote machine. Specifically,
# this is is useful for seting up auto-login for the Naos.
# Instructions filtched from http://linuxproblem.org/art_9.html
#
# NOTE: If you run this script twice on the same robot, your hostname will
# appear twice in .ssh/authorized_keys. Login will still work, but you will
# bloat the file
#
# jstrom, June 2008, updated March 2010
#
##############################################################################

SSH=~/.ssh
ID_RSA_FILE=$SSH/id_rsa
RSA_PUBLIC_KEY_FILE=$SSH/id_rsa.pub

if [ "$1"  = "" ] || [ "$2" = "" ]; then
    echo "Usage: ./setup-autologin <robot-ip> <username>"
    exit 1
fi

echo "Configuring auto login. Only run this once per robot "

if [ ! -e $RSA_PUBLIC_KEY_FILE ] || [ ! -e $ID_RSA_FILE ]; then
    echo "Generating a public key"
    ssh-keygen -t rsa -N "" -f $ID_RSA_FILE
else
    echo "Detected an exisiting public key, so won't generate a new one"
fi

echo "Configuring remote robot with your public key."
echo "Please enter the password for the robot when promted:"
cat $RSA_PUBLIC_KEY_FILE | ssh $2@$1 'mkdir -p .ssh && cat >> .ssh/authorized_keys'

echo "setup complete"
