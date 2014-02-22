# VM

This is a virtual machine for the immutability based operating system [HALT](https://github.com/augustl/halt).

It starts off as a compiler to a bytecode and a bytecode interpreter written in C with the minimal usage of
cstdlib. The goal is to make it simple, operate over a static memory buffer and allow to eventually remove the
dependency on the stdlib. Later, also the compiler can be rewritten without the stdlib.

## Bytecode - executable file format

    <file>          ::= (<header> <version>) <body> # until the first release, the header and version is optional
    <header>        ::= "HALT" # magic
    <version>       ::= <major_version> <minor_version>
    <major_version> ::= <byte>
    <minor_version> ::= <byte>
    <body>          ::= <expression>
    <expression>    ::= "(" <expression>* ")"
                        | "v" <string-16> # symbol reference
                        | "i" <int-32>
                        | "s" <string-32> # string literal
    <int-32>        ::= <byte> <byte> <byte> <byte> # big-endian 32-bit integer
    <int-16>        ::= <byte> <byte> # big-endian 16-bit integer
    <string-16>     ::= <int-16> <byte>{repeating n-times where n is the previous integer}
    <string-32>     ::= <int-32> <byte>{repeating n-times where n is the previous integer}
    <byte>          ::= # 0-255

### Header

Every file starts with the ASCII letters `HALT` followed by two bytes representing a major and minor version. The
changes in minor version are backwards-compatible, whereas when the interpreter encounters an unexpected major version,
it must stop and report an error.

The rest of the file is the single expression to evaluate. The program must contain exactly one expression which is
evaluated. The result of this expression must be a number between 0 and 200. The number is treated as the exit code.

Exit code 0 means success. Exit codes over 200 are reserved for error statuses reported by the VM.

The exit code 253 means normal termination, but failure to provide a correct exit code.
The exit code 254 means abnormal termination due to a VM error.
The exit code 255 means abnormal termination due to a program error.

### Expression

The format is quite high-level and closely matches the processed source code. It has no support for macros as they have
been expanded by the compiler.

The expression is recognized by its first byte.

## Language

The aim is to achieve a strict subset of R5RS Scheme.

`()` - nil (a list with no elements)

`(xxx arguments)` - Evaluates the function xxx with list of arguments.

### Special forms

`(lambda (arguments) body)` - Constructs a function with a lexical scope.

`(if test consequent alternate?)` - If `test` evaluates to true, evaluates to `consequent`, otherwise to `alternate`.

`(quote x)` - Evaluates to the _symbol_ `x`.

`(unquote x)` - Evaluates to the value of `x`.

### Standard functions

`(cons a (cons ()))` - Constructs a list of one element - `a`.
                       It has exactly two arguments, the first one being an arbitrary object, the second one always
                       a list.

`(car (cons a (cons ()))) => a`  - Evaluates to the head of the list supplied as argument.

`(cdr (cons a (cons ()))) => ()` - Evaluates to all but the head of the argument.

`(+ number...)` - Evaluates to sum of the arguments.
