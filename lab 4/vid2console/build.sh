#!/usr/bin/bash
set -e

# Fill this script to build the project. Run this script in the vid2console folder.

# Update package lists
sudo pacman -Syu --noconfirm

# Install ffmpeg and related libraries
sudo pacman -S --noconfirm ffmpeg

# Install OpenCV and development headers
sudo pacman -S --noconfirm opencv

# Install Build Essentials (GCC, G++, Make , etc.)
sudo pacman -S --noconfirm base-devel

OBJ_DIR="build/obj"
BIN_DIR="build/bin"
INCLUDE_DIR="include"

OPENCV_COMPILE_FLAGS=$(pkg-config --cflags opencv4)
OPENCV_LINKING_FLAGS=$(pkg-config --libs opencv4)
CXX_FLAGS=""


for arg in "$@"; do
    if [[ $arg == "clean" ]]; then
        if ((ls build)); then
            rm -rf build
        fi
    fi

    if [[ $arg == "debug" ]]; then
        CXX_FLAGS="-g -O0"
    fi

    if [[ $arg == "release" ]]; then
        CXX_FLAGS="-s -O3"
    fi
done


mkdir -p "$OBJ_DIR"
mkdir -p "$BIN_DIR"

echo "Compiling..."

g++ -c $CXX_FLAGS src/vid2console/main.cpp -o "$OBJ_DIR/main.o" -I include \
$OPENCV_COMPILE_FLAGS -std=c++17

g++ -c $CXX_FLAGS lib/printer.cpp -o "$OBJ_DIR/printer.o" -I include \
$OPENCV_COMPILE_FLAGS -std=c++17

g++ -c $CXX_FLAGS lib/renderer.cpp -o "$OBJ_DIR/renderer.o" -I include \
$OPENCV_COMPILE_FLAGS -std=c++17

g++ -c $CXX_FLAGS lib/vid2arr.cpp -o "$OBJ_DIR/vid2arr.o" -I include \
$OPENCV_COMPILE_FLAGS -std=c++17

g++ -c $CXX_FLAGS lib/framegenerator.cpp -o "$OBJ_DIR/framegenerator.o" -I include \
$OPENCV_COMPILE_FLAGS -std=c++17

# g++ -c  $CXX_FLAGS src/vid2console/main.cpp -o "$OBJ_DIR/main.o" -I "$INCLUDE_DIR" $OPENCV_COMPILE_FLAGS -std=c++17 
# g++ -c  $CXX_FLAGS lib/printer.cpp -o "$OBJ_DIR/printer.o" -I "$INCLUDE_DIR" $OPENCV_COMPILE_FLAGS -std=c++17
# g++ -c  $CXX_FLAGS lib/renderer.cpp -o "$OBJ_DIR/renderer.o" -I "$INCLUDE_DIR" $OPENCV_COMPILE_FLAGS -std=c++17
# g++ -c  $CXX_FLAGS lib/vid2arr.cpp -o "$OBJ_DIR/vid2arr.o" -I "$INCLUDE_DIR" $OPENCV_COMPILE_FLAGS -std=c++17
# g++ -c  $CXX_FLAGS lib/framegenerator.cpp -o "$OBJ_DIR/framegenerator.o" -I "$INCLUDE_DIR" $OPENCV_COMPILE_FLAGS -std=c++17



echo "Linking..."

g++ "$OBJ_DIR/main.o" "$OBJ_DIR/printer.o" "$OBJ_DIR/renderer.o" "$OBJ_DIR/vid2arr.o" "$OBJ_DIR/framegenerator.o" $OPENCV_LINKING_FLAGS -o "$BIN_DIR/vid2console" -std=c++17

chmod 777 "$BIN_DIR/vid2console"
