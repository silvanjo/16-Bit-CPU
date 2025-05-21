# Custom 16-Bit CPU Project

A simple yet complete 16-bit CPU architecture designed from scratch, including a custom instruction set, an assembler, and example programs.

## Project Structure

- **ISA**: Custom-designed 16-bit instruction set with support for arithmetic, logic, memory access, branching, and immediate loading.
- **Assembler**: A C-based tool that translates human-readable assembly into 16-bit binary machine code.
- **Test Programs**: Sample `.asm` files to demonstrate and verify CPU functionality.

## Features

- 16-bit fixed-length instruction format
- 4 general-purpose registers (`r0`–`r3`)
- Support for immediate values, memory access, jumps, and branching
- Minimal and readable binary encoding
- Simple assembler written in C

## Usage

### Assemble a program

```bash
gcc -o assembler simple16asm.c
./assembler program.asm program.bin
```

### Example Instructions

```bash
li r1 42       # Load immediate value into register
add r1 r2 r3   # Add r1 and r2, store in r3
store r0 r3 0  # Store result at memory address
jump 0         # Jump to address 0 (halt simulation)
```
