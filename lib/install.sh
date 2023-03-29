#!/bin/bash

git submodule update --init --recursive

mkdir googletest_build
cd googletest_build
cmake -DCMAKE_INSTALL_PREFIX:PATH=$(pwd)/../googletest ../googletest
make
make install
