#!/bin/bash

OPTS="-j${NUM_CORES} osxcross_sdk=darwin20.2"
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
	scons $OPTS $SCONS_OPTS arch=x86_64 \
		|| exit 1
fi
if [ "$ARCH" = arm64 -o "$ARCH" = universal ]; then
	echo "Running: scons $OPTS $SCONS_OPTS arch=arm64..."
	scons $OPTS $SCONS_OPTS arch=arm64 \
		|| exit 1
fi
if [ "$ARCH" = universal ]; then
	lipo -create bin/godot.osx${FN_OPT}.x86_64 bin/godot.osx${FN_OPT}.arm64 -output bin/godot.osx${FN_OPT}.universal
	rm -f bin/godot.osx${FN_OPT}.x86_64
	rm -f bin/godot.osx${FN_OPT}.arm64
fi
$STRIP bin/godot.osx${FN_OPT}.$ARCH

pushd bin

if [ "$TARGET" = "editor" ]; then
	rm -rf osx_tools.app
	cp -r ../misc/dist/osx_tools.app ./
	mkdir osx_tools.app/Contents/MacOS
	mv godot.osx${FN_OPT}.tools.$ARCH osx_template.app/Contents/MacOS/godot_osx_release.64
	zip $BUILD_TYPE.zip $(find osx_tools.app)
	rm -rf osx_tools.app
else
	rm -rf osx_template.app
	cp -r ../misc/dist/osx_template.app ./
	mkdir osx_template.app/Contents/MacOS
	mv godot.osx${FN_OPT}.$ARCH osx_template.app/Contents/MacOS/godot_osx_release.64
	zip $BUILD_TYPE.zip $(find osx_template.app)
	rm -rf osx_template.app
fi

popd

