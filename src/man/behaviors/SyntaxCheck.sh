#!/bin/bash

find . -name "*.py" | xargs python -m py_compile

find . -name "*.pyc" | xargs rm
