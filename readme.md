# scips (or scratch-mips)

This is a 32-bit MIPS I CPU emulator written in Scratch, with a 2.6MB address space. The source code can be loaded into Scratch from the `scips.sb3` file, and is available here: https://scratch.mit.edu/projects/1000840481/.

Upon clicking the green flag, it loads the executable file (represented as a list of big-endian 32-bit integers) located in the `proc:memory:program` variable into virtual memory. You can click "See Inside" on the Scratch project, and then right-click on the `proc:memory:program` variable to import one of the CSVs in this repository (pre-compiled binaries) into scips.

## Writing Programs for scips
#### Requirements
 - A UNIX environment
 - gcc (or any C compiler)

#### Writing a Program for scips in C
The `scratchmips-essentials.h` contains a lot of implementations for useful library functions, like `printf`, `cls`, `puts`, `putint`, etc. Importing from the standard libraries will not work because coprocessor instructions and floating point registers have not been implemented. Also, MIPS uses the `__start` label the same way C uses the `main` label. Putting everything together, here is a sample Fibonacci number program:

```c
#include "scratchmips-essentials.h"

int fib(int n) {
    return n <= 1 ? n : ( fib(n - 1) + fib(n - 2) );
}

int __start() {
    char buf[128];
    gets(buf, 128);
    int n = atoi(buf);
    printf("fib(%d) = %d\n", n, fib(n));
    return 0;
}
```

#### Compiling
There are multiple C -> MIPS compilers out there, but I'll be using the Linux MIPS toolchain. To install (using apt), run this command:

` $ sudo apt install gcc-mips-linux-gnu`

Then, using `"main.c"` as a stand-in for the file you wish to compile, and `"main"` as a stand-in for the output executable filename, run the following command:

` $ mips-linux-gnu-gcc -march=mips1 -mfp32 -mabi=32 -mno-abicalls -mno-explicit-relocs -nostdlib main.c -o main`

#### Converting

In order to import the executable into scips, it first needs to be converted into a CSV. The `mips_to_csv.c` file facilitates this, but first needs to be compiled (no xz shenanigans here):

` $ gcc --std=c11 mips_to_csv.c -o mips_to_csv.o`

Then, call `mips_to_csv.o` with the input executable as the first argument and the output filename as the second argument:

` $ ./mips_to_csv.o main main.csv`

#### Importing 

Then, click "See Inside" on the Scratch Project, right-click on the `proc:memory:program` variable, and import the CSV.

## Supported Syscalls

scips implements the following syscalls from the MARS emulator (https://courses.missouristate.edu/kenvollmar/mars/help/syscallhelp.html): `print string`, `read string`, `sbrk`, `exit`, `print character`, `read character` (blocking), `sleep`, and `random int`.

It also implements a few custom syscalls:
|Service |Code in $v0  | Arguments | Result |
|--|--|--|--|
| `cls` | 0 | | clears `stdout` | 
| `putsym` | 3 | `$a0` = idx of symbol to print | outputs the symbol to `stdout`

This is the list of supported symbols, where setting `$a0` to `0` will output a red heart, and setting `$a0` to `44` will output a brown square: 💖🔴🔺🔻🟥💙🔵🔹🔷🟦💛🟡🎇🎁🟨💚🟢🍏🥦🟩🧡🟠🔸🔶🟧💜🟣🍇🔮🟪🤍⚪❔❕⬜🖤⚫🏴💀⬛🤎🟤💩💼🟫



