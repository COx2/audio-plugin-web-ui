#!/bin/sh

set -e -u -o pipefail

realpath() {
  [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SCRIPT_NAME="$(basename "$(realpath "$0")")"
SCRIPT_DIRECTORY="$(dirname "$(realpath "$0")")"

cd "${SCRIPT_DIRECTORY}/../"

echo "[audio-plugin-web-ui] Configure from preset..."
cmake --preset xcode

echo "[audio-plugin-web-ui] Build Debug version..."
cmake --build --preset xcode-debug

echo "[audio-plugin-web-ui] Build Release version..."
cmake --build --preset xcode-release
