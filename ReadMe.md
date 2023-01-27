projectM Expression Evaluation Library
======================================

This repository contains the source code for the projectM expression evaluation library, which is a cross-platform
compatible reimplementation of Milkdrop's pendant, the NullSoft Expression Evaluation Library 2, or short "ns-eel2".

This library aims at being as portable as possible, sacrificing some performance over portability. The original ns-eel2
library directly assembles machine code from compiled assembler fragments. While this is super performant, the assembler
code needs to be rewritten manually for every target platform and CPU architecture and is really hard to read or edit.

projectM's Expression Evaluation Library was implemented as a separate project and put under the MIT license to make it
useful in other projects, open source and closed source, while the core projectM library stays under the LGPL license.

## Building the Library

### Requirements

If you just want to build the library, no other dependencies besides a working toolchain, CMake and a build processor
like Make, Ninja, Visual Studio or Xcode is required.

To recreate the parser and lexer or to run tests, additional dependencies need to be installed:

- GTest 1.10 or higher to run the unit tests
- BISON 3.8 and FLEX 2.6 to regenerate the code compiler from the .y/.l files.