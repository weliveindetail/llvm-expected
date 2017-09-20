# llvm-expected

Extract of the structured error handling utilities from the LLVM Support library.

## Original authors

**Most of the code in this repository is not my own work**.
I only extracted it from the LLVM libraries in order to make it reusable for 3rd-parties.
The main developer of the error handling utilities is Lang Hames.
Other original contributers include Eugene Zelenko, Mehdi Amini, Torok Edwin, Chris Lattner, 
Alisdair Meredith, Daniel Dunbar, Dan Gohman and Nakamura Takumi.

## Checkout

Cloning the repository may be slow as it contains the entire history of LLVM. Consider making a shallow clone:
```
$ git clone --depth 20 https://github.com/weliveindetail/llvm-expected.git llvm-expected-shallow
Cloning into 'llvm-expected-shallow'...
remote: Counting objects: 27539, done.
remote: Compressing objects: 100% (25431/25431), done.
remote: Total 27539 (delta 3515), reused 13274 (delta 1772), pack-reused 0
Receiving objects: 100% (27539/27539), 37.89 MiB | 1.22 MiB/s, done.
Resolving deltas: 100% (3515/3515), done.
```

## Usage

The author Lang Hames on motivation and basic usage during the 2016 LLVM Developers’ Meeting:
https://www.youtube.com/watch?v=Wq8fNK98WGw

Great up-to-date documentation in the LLVM Programmer's Manual:
https://llvm.org/docs/ProgrammersManual.html#recoverable-errors

## License

This repository is a fork of LLVM, which makes all terms of the LLVM license apply to the code published here.
Please read [LICENSE.TXT](https://github.com/weliveindetail/llvm-expected/blob/master/LICENSE.TXT) for more information.
