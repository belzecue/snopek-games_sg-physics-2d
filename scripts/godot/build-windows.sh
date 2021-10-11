#!/bin/bash

OPTS="-j${NUM_CORES}"

if [ "$BITS" = "32" ]; then
	OPTS="$OPTS bits=32"
else
	OPTS="$OPTS bits=64"
fi

echo "Running: scons $OPTS $SCONS_OPTS..."
scons $OPTS $SCONS_OPTS \
	|| exit 1

mv bin/godot.${PLATFORM}${FN_OPT}${FN_TOOLS}.$BITS.exe bin/$BUILD_TYPE.exe

