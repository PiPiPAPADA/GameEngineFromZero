@echo off
git submodule update --init External/src/libpng
mkdir  External\build\libpng
cd External\build\libpng
cmake -DZLIB_INCLUDE_DIR=../Windows/Windows/include -DZLIB_LIBRARY=../Windows/Windows/lib/zlib -DCMAKE_INSTALL_PREFIX=../../Windows ../../src/libpng  
cmake --build . --config release --target install