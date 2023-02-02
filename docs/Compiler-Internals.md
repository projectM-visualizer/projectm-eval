Compiler Internals
==================

This document describes how the compiler works and how its functionality can be extended. It is aimed at developers
integrating projectM-EvalLib into their own applications and looking for adding functions and new features on top of the
supported set of Milkdrop-compatible expression syntax.

## Scanner

The scanner (or lexer) is responsible for tokenizing the input data stream and return the type and possible content of
these tokens to the parser. This library uses GNU Flex to generate ode from a rules file, called `Scanner.l`. The
scanner looks for these kinds of symbols, with letters always being case-insensitive:

- Inline comments, starting with `//` until end of the current line
- Block comments, enclosed in `/* ... */`, which can appear anywhere and also span multiple lines.
- HEX constants in the form `$Xnn...`, scanned until any non-hexadecimal character is encountered (`[0-9a-f]`)
- ORD constants in the form `$'c'` where `c` is any single character.
- Math constants `$PI`, `$E` and `$PHI`.
- Special functions with different grammar than "normal" functions (loops and memory access functions).
- Numbers in integer and floating-point notation with an optional exponent. Note that these numbers are always positive,
  negativity is applied later via the `-` unary operator in the parser.
- Identifiers starting with an underscore or alphabetical letter, and these characters plus and numbers from the second
  character on. These can be returned as two diffent tokens:
    - If the (lower-case) name exists in the function table, the name is returned as a `FUN` token.
    - Anything else is returned as a `VAR` token and identifies a variable.

## Grammar

## Expression Tree

### Node Object

All grammar as described in the previous sections is scanned and compiled into a tree structure by the parser actions.
Each node is a `prjm_eel_exptreenode` struct, which contains:

- A function pointer `func`, which is determined the behaviour of this node.
- A fixed float value `value` to store constant numbers or temporary values.
- Three argument pointers `arg1`, `arg2` and `arg3` with the first being reused as a pointer to a `prjm_eel_variable`
  under the name `var`.
- A linked-list pointer `next` which stores additional instructions if the object uses arbitrary-length instruction
  lists.

In general, only a few different "objects" will be created as tree nodes:

- Constants: return simple read-only float-typed numbers.
- Variables: return a read/write variable reference.
- Functions: Execute the argument expression objects, apply a function on the results and set the return value to either
  a read-only value or a variable reference.

Loops and instruction lists are also represented by functions and use the `next` member of the object to store the
instruction list to execute.

### Node Function

The function in each node object has a `void` return value and takes two parameters:

- The execution context `ctx`, which is a pointer to the `prjm_eel_exptreenode` object the function will be executed on.
  Think of it as the C++ `this` pointers.
- A pointer to a float pointer `ret_val`, which is the return value of the function.

Every node function _must_ return either a value or a value reference. By convention, this is either the result of the
operation the function carried out, or for functions using instruction lists, the result of the _last executed
instruction_.

Depending on the function, the result can either be a simple constant value or a reference to a varaiable.

When calling a math function like `sin()`, implementing a math operator or returning a constant, the function must
assign the value to the value pointed to by `*ret_val`:

```c
(**ret_val) = sin(x);
```

Some functions will not carry out an actual math operation, but return the result of an expression, which may be a
variable reference, which may be used as the first argument of the internal `_set` function (or the
AVS-specific `assign` function) to assign it the result of another expression. A reference can be assigned this way,
example taken from the `prjm_eel_func_var` function:

```c
(*ret_val) = ctx->var->value_ptr;
```

As with all pointers, the assigned pointer must stay valid at least until the end of the program's execution. It is
recommended to only use the following two memory references:

- The `value_ptr` value of any registered `prjm_eel_variable` object.
- The address of the `value` member of the current `prjm_eel_exptreenode` object in which context the function runs.

The `if` function evaluates its first argument, and depending on it being a true or false value, returns either the
second (`true`) or the third (`false`) argument. Both arguments may return a variable reference, so the `if` functions
needs to take care of this by simply calling the appropriate function and passing the current `ret_val` to it. This way,
whatever the sub-expression assign to it will be returned by the `if` function.

#### Compile-time Evaluable Functions

The compiler optimizes the generated code by evaluating constant expressions at compile time. These expressions purely
consist of functions which both only have constant-evaluable arguments and have a fixed, deterministic behaviour.
Examples of such functions, given their arguments are also const-evaluable:

- Functions which return compile-time constants.
- Non-assigment Operators.
- Math functions like `sin`.

Non-constant functions are functions which depend on variables or other memory contents which may change during or
between executions or produce a non-deterministic result on each execution. Examples are:

- Functions which have at least one non-const-evaluable argument.
- Variable and memory access functions.
- Random number generators.

During compilation, when a new node is created from a subtree, each argument and the new node are checked for being
const-evaluable. If all are, then the new node is evaluated by calling the function, and a new 