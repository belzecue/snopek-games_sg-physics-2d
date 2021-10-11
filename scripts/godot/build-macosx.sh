#!/bin/bash

OPTS="-j${NUM_CORES} platform=osx tools=no target=release production=yes osxcross_sdk=darwin20.2"
STRIP="x86_64-apple-darwin20.2-strip -u -r"

case "$BUILD_TYPE" in
	macosx-x86-64)
		ARCH=x86_64
		;;
	macosx-arm64)
		ARCH=arm64
		;;
	macosx-universal)
		ARCH=universal
		;;
	*)
		echo "Unkown BUILD_TYPE: $BUILD_TYPE"
		exit 1
		;;
esac

if [ "$ARCH" = x86_64 -o "$ARCH" = universal ]; then
	echo "Running: scons $OPTS $SCONS_OPTS arch=x86_64..."
	scons $OPTS $SCONS_OPTS arch=x86_64
fi
if [ "$ARCH" = arm64 -o "$ARCH" = universal ]; then
	echo "Running: scons $OPTS $SCONS_OPTS arch=arm64..."
	scons $OPTS $SCONS_OPTS arch=arm64
fi
if [ "$ARCH" = universal ]; then
	lipo -create bin/godot.osx.opt.x86_64 bin/godot.osx.opt.arm64 -output bin/godot.osx.opt.universal
fi
$STRIP bin/godot.osx.opt.$ARCH

pushd bin
rm -rf osx_template.app
cp -r ../misc/dist/osx_template.app ./
mkdir osx_template.app/Contents/MacOS
cp godot.osx.opt.$ARCH osx_template.app/Contents/MacOS/godot_osx_release.64
zip osx.zip $(find osx_template.app)
rm -rf osx_template.app
popd

