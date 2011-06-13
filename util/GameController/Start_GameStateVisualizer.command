#!/bin/bash
cd `dirname $0`

set -eu

echo Starting GameStateVisualizer

java -jar GameStateVisualizer.jar -spl -fullscreen &
