#!/bin/sh

BINARY="elevator.bin"
BUILD_TARGET="Debug"
FLASH_START_ADDR=0x8000000

st-flash write $BUILD_TARGET/$BINARY $FLASH_START_ADDR
