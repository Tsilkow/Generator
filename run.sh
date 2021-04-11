#!/bin/bash

cp -r data/ build/data/
clear
cd build/
cmake ..
if eval cmake --build .; then
    if [ "$#" -ge 1 ]; then
	exec "./Generator" "$1"
    else
	exec "./Generator"
    fi
    cd ..
else
    cd ..
fi
