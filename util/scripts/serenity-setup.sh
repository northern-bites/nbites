#!/bin/bash

echo "Note: if prompted for a password, it's probably hotdawgs."

pushd ~/
scp .ssh/id_rsa.pub northern-bites@serenity.campus.bowdoin.edu:.ssh/temp

ssh northern-bites@serenity.campus.bowdoin.edu 'cat .ssh/temp >> .ssh/authorized_keys2 && rm .ssh/temp'

popd
