#!/bin/bash

OPTS="-j${NUM_CORES}"

if [ "$BITS" = "32" ]; then
	export PATH="${GODOT_SDK_LINUX_X86}/bin:${BASE_PATH}"
	OPTS="$OPTS bits=32"
else
	export PATH="${GODOT_SDK_LINUX_X86_64}/bin:${BASE_PATH}"
	OPTS="$OPTS bits=64"
fi

echo "Running: scons $OPTS $SCONS_OPTS..."
if scons $OPTS $SCONS_OPTS; then
	mv bin/godot.${PLATFORM}${FN_OPT}${FN_TOOLS}.$BITS bin/$BUILD_TYPE
	exit 0
fi

exit 1
