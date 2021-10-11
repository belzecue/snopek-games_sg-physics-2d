#!/bin/bash

GODOT_SOURCE_DIR=${GODOT_BUILD_DIR:-godot}
GODOT_BUILD_DIR=${GODOT_BUILD_DIR:-build/godot}

die() {
	echo "$@" > /dev/stderr
	exit 1
}

if [ -z "$BUILD_TYPE" ]; then
	die "The BUILD_TYPE environment variable must be set"
fi

if [ ! -d "$GODOT_SOURCE_DIR" ]; then
	die "No such GODOT_SOURCE_DIR diretory at $GODOT_SOURCE_DIR"
fi

if [ ! -d "$GODOT_SOURCE_DIR/modules" ]; then
	die "GODOT_SOURCE_DIR diretory at $GODOT_SOURCE_DIR has no 'modules' sub-directory"
fi

if [ -z "$GODOT_DOWNLOAD_URL" ]; then
	die "Source directory is missing required GODOT_DOWNLOAD_URL file"
fi

if [ ! -d "$GODOT_BUILD_DIR" ]; then
	mkdir -p "$GODOT_BUILD_DIR" \
		|| die "Unable to create GODOT_BUILD_DIR: $GODOT_BUILD_DIR"

	(cd "$GODOT_BUILD_DIR" && curl -L "$GODOT_DOWNLOAD_URL" | tar -xz --strip-components=1) \
		|| die "Unable to download Godot source from GODOT_DOWNLOAD_URL: $GODOT_DOWNLOAD_URL"
else
	echo " !! WARNING: Reusing existing build directory !! "
fi

if [ -z "$NUM_CORES" ]; then
	NUM_CORES=$(nproc --all)
fi

PRIVATE_IMAGE="no"
IMAGE=""
CMD=""

case "$BUILD_TYPE" in
	server-tools|server)
		IMAGE="godot-linux"
		CMD="build-server.sh"
		;;
	linux-32|linux-64)
		IMAGE="godot-linux"
		CMD="build-linux.sh"
		;;
	windows-32|windows-64)
		IMAGE="godot-windows"
		CMD="build-windows.sh"
		;;
	macosx-x86-64|macosx-arm64|macosx-universal)
		IMAGE="godot-osx"
		PRIVATE_IMAGE="yes"
		CMD="build-macosx.sh"
		;;
	html5)
		IMAGE="godot-javascript"
		CMD="build-html5.sh"
		;;
	*)
		die "Unknown BUILD_TYPE: $BUILD_TYPE"
		;;
esac

BITS=""
case "$BUILD_TYPE" in
	*-32*)
		BITS="32"
		;;
	*-64*)
		BITS="64"
		;;
esac

MONO=""
case "$BUILD_TYPE" in
	*-mono*)
		MONO="yes"
		;;
esac

TOOLS=""
case "$BUILD_TYPE" in
	*-tools*)
		TOOLS="yes"
		;;
esac

SCONS_OPTS=${SCONS_OPTS:-}
SCONS_OPTS="$SCONS_OPTS custom_modules=/src/modules"

if [ -n "$GODOT_BUILD_REGISTRY" ]; then
	# In the registry, godot-linux becomes godot/linux.
	IMAGE=$(echo "$IMAGE" | sed -e 's,-,/,')
	IMAGE="$GODOT_BUILD_REGISTRY/$IMAGE"
	if [ "$PRIVATE_IMAGE" = yes ]; then
		IMAGE=$(echo "$IMAGE" | sed -e 's,/godot/,/godot-private/,')
	fi
fi
if [ -n "$GODOT_BUILD_TAG" ]; then
	IMAGE="$IMAGE:$GODOT_BUILD_TAG"
fi

PODMAN_OPTS=${PODMAN_OPTS:-}

podman run --rm --systemd=false -v "$(realpath $GODOT_BUILD_DIR):/build" -v "$(realpath $GODOT_SOURCE_DIR):/src" -v "$(pwd)/scripts/godot:/scripts" -w /build -e NUM_CORES="$NUM_CORES" -e BITS="$BITS" -e MONO="$MONO" -e TOOLS="$TOOLS" -e "SCONS_OPTS=$SCONS_OPTS" -e BUILD_TYPE=$BUILD_TYPE $PODMAN_OPTS "$IMAGE" /scripts/$CMD $BUILD_TYPE
exit $?

