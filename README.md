# Limit-Order-Book-Matching-engine
A simple (kind of) limit order book with a matching engine in C++

Should now be functional.

Requires CMake:
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

In the CMAKE txt file, you need to edit the path to the TBB CMake file.
The CMake files can be downloaded from: https://www.intel.com/content/www/us/en/developer/tools/oneapi/onetbb-download.html
