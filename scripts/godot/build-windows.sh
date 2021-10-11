#!/bin/bash

OPTS="-j${NUM_CORES} platform=windows tools=no target=release production=yes"

if [ "$BITS" = "32" ]; then
	OPTS="$OPTS bits=32"
else
	OPTS="$OPTS bits=64"
fi

echo "Running: scons $OPTS $SCONS_OPTS..."
exec scons $OPTS $SCONS_OPTS

