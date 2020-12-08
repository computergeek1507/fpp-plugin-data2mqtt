#!/bin/sh

echo "Running fpp-data2mqtt PreStart Script"

BASEDIR=$(dirname $0)
cd $BASEDIR
cd ..
make
