#!/bin/bash
#
# Kill webots once a minute, to hack around not having access
# to wb_supervisor_simulation_revert()

set -e

while :
do
    /Applications/Webots/webots & # --minimize
    sleep 70s
    killall -9 nao_team_1
    killall -9 webots
    sleep 2s
done