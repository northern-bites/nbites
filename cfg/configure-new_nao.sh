#!/bin/sh

sh install-python.sh $1

scp .bashrc root@$1:

#sh configure-wireless.sh $1