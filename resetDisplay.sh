#!/bin/bash
######################
#
#
#
######################


SERIAL_PORT="/dev/ttyUSB3"

./HanshowFlasher -b115200 -c${SERIAL_PORT} -r

