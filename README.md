## DUB Language
The dub language interpreter and compiler, written in C.

## How to
### Using CMake
> cd build
> cmake ..
> make
> ./dub my_code.dub

### Using raw gcc
> gcc -g -O0 logging.c lexer.c state.c ast.c eval.c main.c -o dub
> ./dub my_code.dub
