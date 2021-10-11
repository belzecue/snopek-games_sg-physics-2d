#!/bin/bash

OPTS="-j${NUM_CORES} platform=x11 production=yes"

if [ "$BITS" = "32" ]; then
	export PATH="${GODOT_SDK_LINUX_X86}/bin:${BASE_PATH}"
	OPTS="$OPTS bits=32"
else
	export PATH="${GODOT_SDK_LINUX_X86_64}/bin:${BASE_PATH}"
	OPTS="$OPTS bits=64"
fi

echo "Building editor..."
scons $OPTS $SCONS_OPTS tools=yes target=release_debug \
	|| exit 1

echo "Building export templates..."
scons $OPTS $SCONS_OPTS tools=no target=release \
	|| exit 1

