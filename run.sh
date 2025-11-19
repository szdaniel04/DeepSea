#!/bin/bash

# --- PROJECT ROOT ---
ROOT_DIR="$(cd "$(dirname "$0")"; pwd)"

echo ">>> Project root: $ROOT_DIR"

# --- BUILD DIR ---
BUILD_DIR="$ROOT_DIR/build"

# check build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo ">>> ERROR: build mappa nem létezik!"
    echo ">>> Hozz létre egy build mappát így:"
    echo "mkdir build && cd build && cmake .."
    exit 1
fi

# --- UPDATE BUILD (CMake + make) ---
cd "$BUILD_DIR"

echo ">>> Running CMake (update)..."
cmake .. || { echo ">>> CMake failed!"; exit 1; }

echo ">>> Building project..."
make -j$(nproc) || { echo ">>> Build failed!"; exit 1; }

# --- COPY SHADERS & ASSETS ---
echo ">>> Syncing Shaders/ and Assets/... (root → build)"
rsync -a "$ROOT_DIR/Shaders" "$BUILD_DIR/"
rsync -a "$ROOT_DIR/Assets" "$BUILD_DIR/"

# --- RUN FROM ROOT ---
cd "$ROOT_DIR"
echo ">>> Running program from project root..."
./build/ZH_Base

