#!/bin/bash

mkdir -p build && cd build && cmake -G Ninja -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="include-what-you-use" ..
cd ..
cmake --build ./build --target clean
cmake --build ./build --target jcc | tee /tmp/iwyu.out
fix_includes.py --comments --update_comments --nosafe_headers < /tmp/iwyu.out
find src/ -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i
