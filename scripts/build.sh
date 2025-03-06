#!/bin/sh
set -e # Exit on failure

cd "$(dirname "$0")/.."
cmake -B build -S . 
cmake --build ./build