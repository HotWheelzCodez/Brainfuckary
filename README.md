# Brainfuckary
A basic brainfuck interpreter in C++

# Compile
Use g++ to compile this, this program uses the C++ standard 11
```bash
g++ brainfuck.cpp -O3 -std=c++11 -o brainfuck
```

# Running the program
To use this brainfuck interpreter, just call the executable file that you compiled with the given brainfuck program you have created
## Windows
```bash
brainfuck file.bf
```
or
## Unix
```bash
./brainfuck file.bf
```

## Flags
There are also a few flag optins for this the two that I have right now are "*--message-off*" which disables the inital display message, and there is "*--cells=100*" (or any number of cells/bytes that you need), however make sure to put the flags after the input file, and to only provide one input file. 
```bash
brainfuck test/add.bf --message-off
```
