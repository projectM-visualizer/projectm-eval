projectM Expression Evaluation Library
======================================

This repository contains the source code for the projectM Expression Evaluation Library, short "projectm-eval", which is
a cross-platform and cross-architecture reimplementation of Milkdrop's pendant, the NullSoft Expression Evaluation
Library 2, or short "ns-eel2".

This library aims at being as portable as possible, sacrificing some performance over portability. The original ns-eel2
library directly assembles machine code from compiled assembler fragments. While this is super performant, the assembler
code needs to be rewritten manually for every target platform and CPU architecture and is really hard to read or edit.

projectM's Expression Evaluation Library was implemented as a separate project and put under the MIT license to make it
useful in other projects, open source and closed source, while the core projectM library stays under the LGPL license.

Implementation and usage details can be found in the ["docs" subdirectory](docs).

## Building the Library

### Requirements

If you just want to build the library, no other dependencies besides a working toolchain, CMake (3.20 or higher) and a
build processor like Make, Ninja, Visual Studio or Xcode are required.

To recreate the parser and lexer or to run tests, additional dependencies need to be installed:

- Bison 3.8 and Flex 2.6 to regenerate the code compiler from the .y/.l files.
- GTest 1.10 or higher to run the unit tests
- Google Benchmark to run the benchmarks.

To specify custom locations for Bison and Flex, set BISON_EXECUTABLE and FLEX_EXECUTABLE accordingly when configuring
the project.
