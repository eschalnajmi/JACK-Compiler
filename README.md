# JACK Compiler

This is a compiler for the JACK language written in C as part of the University of Leeds Compiler Design and Construction module.

## Description

This code is able to create lexemes of code, parse through the code, generate a symbol table for a directory & generate .vm files.

## Getting Started

### Executing program

* Type this in terminal (macOS):
```
gcc compiler.c parser.c lexer.c symbols.c -o compile
./compile
```
* Type this in terminal (linux):
```
gcc compiler.c compiler.h parser.c paresr.h lexer.c lexer.h symbols.c symbols.h -o compile
./compile
```
## Acknowledgments

This code was written using test script code by [Dr Mohammad Ammar Alsalka](https://eps.leeds.ac.uk/computing/staff/635/dr-mohammad-ammar-alsalka) which has been removed in the main branch. Parser.h and Lexer.h were also provided by him.
