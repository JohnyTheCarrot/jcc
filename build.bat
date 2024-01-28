mkdir build
cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_GENERATOR=Ninja .. && cmake --build . -j 4 && cd ..
