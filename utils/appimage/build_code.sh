#!/bin/bash
set -e

mkdir build
cd build
cmake -DCMAKE_CXX_STANDARD=17 ..
make
cd ..