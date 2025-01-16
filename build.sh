#!/usr/bin/env sh
set -ex

cmake -S . -B .build
cmake --build .build

if [ "$#" -gt 0 ]; then
    if [ "$1" = "-r" ]; then
        ./.build/tyger_test
    fi
fi
