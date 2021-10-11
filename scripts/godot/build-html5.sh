#!/bin/bash

OPTS="-j${NUM_CORES}"

if [ "$TARGET" = "export-template-release" ]; then
	OPTS="$OPTS use_closure_compiler=yes"
fi

source /root/emsdk_${EMSCRIPTEN_CLASSICAL}/emsdk_env.sh

echo "Running: scons $OPTS $SCONS_OPTS..."
scons $OPTS $SCONS_OPTS \
	|| exit 1

mv bin/godot.${PLATFORM}.${FN_OPT}.zip bin/$BUILD_TYPE.zip

