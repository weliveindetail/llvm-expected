[![Build Status](https://travis-ci.org/weliveindetail/llvm-expected.svg?branch=master)](https://travis-ci.org/weliveindetail/llvm-expected)

# llvm-expected

LLVM's Rich Recoverable Error Handling as a C++17 Header-Only Library.

## Original authors

**Most of the code in this repository is not my own work**.
I only extracted it from the LLVM libraries in order to make it usable for 3rd-parties.

## Checkout

The current `master` is based on the upcoming LLVM 7.0 Release and requires a compiler with support for [C++17 Inline Variables](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4424.pdf). 
Please find the old not-header-only version on the [release_50](https://github.com/weliveindetail/llvm-expected/tree/release_50) branch.

In order to use the library you can just copy the
[llvm-expected.h](https://raw.githubusercontent.com/weliveindetail/llvm-expected/master/llvm-expected.h) to your project.
I will cherry-pick fixes for this release from LLVM if there are any. You may want checkout the `release_70` branch 
in a git submodule to easily pull. Consider a shallow clone as the repo contains the entire history of LLVM:
```
$ git submodule add --depth=1 --branch=release_70 https://github.com/weliveindetail/llvm-expected.git
```

## Build Tests

Make a recursvie clone to include gtest and run the unit tests:
```
$ git clone --recursive --depth 1 https://github.com/weliveindetail/llvm-expected.git
$ mkdir build-test
$ cd build-test
$ cmake -G Ninja ../llvm-expected
$ cmake --build .
$ ./llvm-expected-test
```

## Usage

The author Lang Hames on motivation and basic usage during the 2016 LLVM Developers’ Meeting:<br>
https://www.youtube.com/watch?v=Wq8fNK98WGw

Great up-to-date documentation in the LLVM Programmer's Manual:<br>
https://llvm.org/docs/ProgrammersManual.html#recoverable-errors

Some time ago I wrote a blog post with a motivation:<br>
https://weliveindetail.github.io/blog/post/2017/09/06/llvm-expected-basics.html

## License

This repository is a fork of LLVM, which makes all terms of the LLVM license apply to the code published here.
Please read [LICENSE.TXT](https://github.com/weliveindetail/llvm-expected/blob/master/LICENSE.TXT) for more information.
