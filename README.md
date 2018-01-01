[![Build Status](https://travis-ci.org/weliveindetail/llvm-expected.svg?branch=master)](https://travis-ci.org/weliveindetail/llvm-expected)

# llvm-expected

LLVM's Rich Recoverable Error Handling as a Library

## Original authors

**Most of the code in this repository is not my own work**.
I only extracted it from the LLVM libraries in order to make it reusable for 3rd-parties.
The main developer of the error handling utilities is Lang Hames.
Other original contributors include Eugene Zelenko, Mehdi Amini, Torok Edwin, Chris Lattner, 
Alisdair Meredith, Daniel Dunbar, Dan Gohman and Nakamura Takumi.

## Checkout

This repo contains the entire history of LLVM, consider making a shallow clone:
```
$ git clone --recursive --depth 20 https://github.com/weliveindetail/llvm-expected.git llvm-expected
Cloning into 'llvm-expected'...
remote: Counting objects: 27539, done.
remote: Compressing objects: 100% (25431/25431), done.
remote: Total 27539 (delta 3515), reused 13274 (delta 1772), pack-reused 0
Receiving objects: 100% (27539/27539), 37.89 MiB | 1.22 MiB/s, done.
Resolving deltas: 100% (3515/3515), done.
```

## Build Tests
```
$ ls
llvm-expected
$ mkdir build-test
$ cd build-test
$ cmake -G <generator> -DCMAKE_BUILD_TYPE=Debug -DLLVMEXPECTED_INCLUDE_TESTS=ON ../llvm-expected
$ cmake --build .
$ ./tests/TestLLVMExpected
```

## Build Benchmarks
```
$ ls
llvm-expected
$ mkdir build-bench
$ cd build-bench
$ cmake -G <generator> -DCMAKE_BUILD_TYPE=Release -DLLVMEXPECTED_INCLUDE_BENCHMARKS=ON ../llvm-expected
$ cmake --build .
$ ./benchmarks/LLVMExpected/BenchmarkLLVMExpected
$ ./benchmarks/Exception/BenchmarkException
```

## Usage

The author Lang Hames on motivation and basic usage during the 2016 LLVM Developers’ Meeting:<br>
https://www.youtube.com/watch?v=Wq8fNK98WGw

Great up-to-date documentation in the LLVM Programmer's Manual:<br>
https://llvm.org/docs/ProgrammersManual.html#recoverable-errors

## License

This repository is a fork of LLVM, which makes all terms of the LLVM license apply to the code published here.
Please read [LICENSE.TXT](https://github.com/weliveindetail/llvm-expected/blob/master/LICENSE.TXT) for more information.
