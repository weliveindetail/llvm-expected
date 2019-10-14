[![Build Status](https://travis-ci.org/weliveindetail/llvm-expected.svg?branch=master)](https://travis-ci.org/weliveindetail/llvm-expected)

# llvm-expected

LLVM's Rich Recoverable Error Handling as a C++17 Header-Only Library.

## Original authors

**Most of the code in this repository is not my own work**.
I only extracted it from the LLVM libraries in order to make it usable for 3rd-parties.

## Example

```cpp
#include <iostream>
#include <sstream>

#include "llvm-expected.h"

llvm::Expected<int> to_int(const std::string &S, bool MatchWord = false) {
  std::istringstream ss(S);
  ss.imbue(std::locale::classic());

  int result;
  ss >> result;

  if (LLVM_UNLIKELY(ss.fail()))
    return llvm::createStringError("Failed to parse int: " + S);

  if (LLVM_UNLIKELY(MatchWord && !ss.eof()))
    return llvm::createStringError("Failed to parse full string as int: " + S);

  return result;
}

int main(int argc, char *argv[]) {
  llvm::Error Errs = llvm::Error::success();
  auto QueueError = [&Errs](llvm::Error Err) {
    Errs = llvm::joinErrors(std::move(Errs), std::move(Err));
  };

  std::cout << "Ints: ";
  static constexpr bool MatchWord = true;

  for (int i = 1; i < argc; i++) {
    if (auto parsed_int = to_int(argv[i], MatchWord)) {
      std::cout << *parsed_int << " ";
    } else {
      QueueError(parsed_int.takeError());
    }
  }

  std::cout << "\n";
  llvm::logAllUnhandledErrors(std::move(Errs), std::cout, "\nErrors:\n");
  std::cout << "\n";
  return 0;
}
```

## Build and run example

```
$ git clone --depth 1 https://github.com/weliveindetail/llvm-expected.git
$ clang -std=c++17 -lc++ llvm-expected/example.cpp -o example
$ ./example 4
Ints: 4 

$ ./example _ 42 -0 1a +4 b5
Ints: 42 0 4 

Errors:
Failed to parse int from '_'
Failed to parse full string as int in '1a'
Failed to parse int from 'b5'

```

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

## Usage

The author Lang Hames on motivation and basic usage during the 2016 LLVM Developers’ Meeting:<br>
https://www.youtube.com/watch?v=Wq8fNK98WGw

Great up-to-date documentation in the LLVM Programmer's Manual:<br>
https://llvm.org/docs/ProgrammersManual.html#recoverable-errors

Some time ago I wrote a blog post with a motivation:<br>
https://weliveindetail.github.io/blog/post/2017/10/22/llvm-expected.html

## Build Tests

Make a recursvie clone to include gtest and run the unit tests:
```
$ git clone --recursive --depth 1 https://github.com/weliveindetail/llvm-expected.git
$ cd llvm-expected
$ git submodule update --init --recursive
$ mkdir ../build-test
$ cd ../build-test
$ cmake -G Ninja ../llvm-expected
$ cmake --build .
$ ./llvm-expected-test
```

## License

This repository is a fork of LLVM, which makes all terms of the LLVM license apply to the code published here.
Please read [LICENSE.TXT](https://github.com/weliveindetail/llvm-expected/blob/master/LICENSE.TXT) for more information.
