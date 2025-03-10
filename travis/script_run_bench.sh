#!/usr/bin/env bash

set -e
set -o pipefail

if [[ ${PLATFORM} == "osx" || ${PLATFORM} == "linux" ]]; then
    # Run unit tests
    echo "Running Benchmarks"
    pushd ./build/bench/bin

    for file in *.out
        do
            echo "Running ${file}"
            ./$file
        done
    popd
fi
