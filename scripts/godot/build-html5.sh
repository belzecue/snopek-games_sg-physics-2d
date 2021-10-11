#!/bin/bash

OPTS="-j${NUM_CORES} platform=javascript tools=no target=release production=yes use_closure_compiler=yes"

source /root/emsdk_${EMSCRIPTEN_CLASSICAL}/emsdk_env.sh

echo "Running: scons $OPTS $SCONS_OPTS..."
exec scons $OPTS $SCONS_OPTS

