#!/bin/sh

BINARY_BASE="elevator.binary"
BINARY_NEW="elevator.bin"
BUILD_TARGET="Debug"

cd ..
PWD=$(pwd)

if [ -f $PWD/$BUILD_TARGET/$BINARY_BASE ]; then
   mv $PWD/$BUILD_TARGET/$BINARY_BASE $PWD/$BUILD_TARGET/$BINARY_NEW
fi
