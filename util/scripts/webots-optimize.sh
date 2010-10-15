#!/bin/bash
#
# Kill webots once a minute, to hack around not having access
# to wb_supervisor_simulation_revert()

set -e

while :
do
    webots &
    sleep 70s
    killall nao_team_1
    killall webots-bin
    sleep 3s
done