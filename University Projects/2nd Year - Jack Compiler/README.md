# Overview
This project was inspired by the Jack code introduced as part of the [nand2tetris](https://www.nand2tetris.org/) project. \
By default, `compiler.c` will compile the contents of the `Pong` directory, but this can be configured on line 133.
The `.jack` files were included as part of the compilation process, while the `Average/` `ComplexArrays/` `Pong/` `Square/` and `StringTest/` directories were provided as a means of testing various stages of the compiler.
Any C files or header files included were either provided as barebones skeleton codes or developed from the ground-up myself.

# Running the Jack Compiler
This program was designed to run on linux.\
Enter the following into the terminal:
```
cc std=c99 symbols.h compiler.h lexer.c parser.c symbols.c compiler.c
./a.out
```