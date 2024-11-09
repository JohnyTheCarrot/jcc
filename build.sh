mkdir -p build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G Ninja ..
cmake --build . --target jcc -j 4
cd ..
