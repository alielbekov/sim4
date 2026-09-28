# SIM4 — Single-Cycle MIPS CPU Simulator

A C implementation of parts of a **single-cycle MIPS CPU simulator** developed for **CSc 252**.

The project models the main stages of executing MIPS instructions: fetching an instruction, decoding its fields, generating CPU control signals, performing ALU and memory operations, updating the program counter, and writing results back to registers.

## Features

- 32-bit MIPS instruction decoding
- CPU control signal generation
- Sign extension for 16-bit immediates
- Instruction fetching from simulated instruction memory
- Support for common MIPS instructions used by the assignment, including:
  - `ADD`, `ADDU`
  - `SUB`, `SUBU`
  - `AND`, `OR`, `XOR`
  - `SLT`, `SLTI`
  - `ADDI`, `ADDIU`
  - `BEQ`
  - `J`
  - `LW`
  - `SW`
- Test infrastructure for individual CPU stages and complete instruction execution
- Automated grading script that compiles tests and compares their output with expected results

## CPU Execution Flow

The simulator follows the major stages of a single-cycle CPU:

```text
Instruction Memory
       |
       v
+-------------------+
| Fetch Instruction |
+-------------------+
       |
       v
+-------------------+
| Decode Fields     |
| + Control Signals |
+-------------------+
       |
       v
+-------------------+
| Select ALU Inputs |
+-------------------+
       |
       v
+-------------------+
| Execute ALU       |
+-------------------+
       |
       v
+-------------------+
| Memory Access     |
+-------------------+
       |
       v
+-------------------+
| Update PC         |
+-------------------+
       |
       v
+-------------------+
| Write Registers   |
+-------------------+
```

The public interface in `sim4.h` separates these stages into functions such as:

```c
getInstruction(...)
extract_instructionFields(...)
fill_CPUControl(...)
getALUinput1(...)
getALUinput2(...)
execute_ALU(...)
execute_MEM(...)
getNextPC(...)
execute_updateRegs(...)
```

This makes it possible to test each part of the simulated datapath independently.

## Project Structure

```text
sim4/
├── sim4.c
├── sim4.h
├── sim4_test_commonCode.c
├── sim4_test_commonCode.h
├── grade_sim4
├── extra_testcases/
├── test01
├── test02
├── test_01_getInstruction.c
├── test_02_executeControl1.c
├── test_03_executeControl2.c
├── test_04_aluInputs.out
├── test_05_execALU.c
├── test_06_execMEM.c
├── test_07_fullInstructions.c
├── test_08_checkUpper4BitsOnJump.c
├── test_09_singleInstruction.c
├── test_10_syscalls.c
├── test_11_multipleInstructions1.c
├── test_12_multipleInstructions1-noDebug.c
├── test_13_invalidInstructions.c
├── test_20_ADD_SUB_ADDI.c
├── test_21_AND_OR_XOR.c
└── test_22_SLT_SLTI.c
```

### Important Files

| File | Purpose |
| --- | --- |
| `sim4.c` | Main student implementation |
| `sim4.h` | CPU structures and function declarations |
| `sim4_test_commonCode.c` | Shared simulator/testing infrastructure |
| `sim4_test_commonCode.h` | Shared test declarations |
| `grade_sim4` | Automated grading/test script |
| `test_*.c` | Individual test programs |
| `test_*.out` | Expected output for tests |
| `extra_testcases/` | Additional test cases |

## Current `sim4.c` Implementation

The current implementation includes:

### `getInstruction`

Fetches a 32-bit instruction from instruction memory based on the program counter.

```c
index = curPC / 4;
```

Because MIPS instructions are four bytes wide, the PC is divided by four to obtain the instruction-memory index.

### `extract_instructionFields`

Splits a 32-bit MIPS instruction into its fields:

- `opcode`
- `rs`
- `rt`
- `rd`
- `shamt`
- `funct`
- `imm16`
- sign-extended `imm32`
- 26-bit jump `address`

### `fill_CPUControl`

Generates the control signals required for the decoded instruction.

The `CPUControl` structure contains signals such as:

```text
ALUsrc
ALU.op
ALU.bNegate
memRead
memWrite
memToReg
regDst
regWrite
branch
jump
```

The function returns `0` when an opcode/function combination is not recognized and `1` when the instruction is supported.

## Building

The test suite uses GCC with GNU99.

### Requirements

- GCC
- Bash
- GNU `timeout`
- Standard Unix utilities such as `diff`, `grep`, `head`, and `wc`

On Linux, these tools are normally already available.

On macOS, the grading script may require GNU coreutils because macOS does not provide the GNU `timeout` command by default.

```bash
brew install coreutils
```

Depending on your setup, GNU timeout may be available as `gtimeout` instead of `timeout`.

## Clone the Repository

```bash
git clone https://github.com/alielbekov/sim4.git
cd sim4
```

## Running the Tests

The repository includes the `grade_sim4` grading script.

Make it executable if necessary:

```bash
chmod +x grade_sim4
```

Then run:

```bash
./grade_sim4
```

The script automatically finds the `test_*.c` files, compiles each testcase together with:

```text
sim4.c
sim4_test_commonCode.c
```

and compares the program output against the corresponding `.out` file.

For C tests, the grading script effectively performs compilation similar to:

```bash
gcc -g -std=gnu99 sim4.c sim4_test_commonCode.c test_name.c -lm -o test_name
```

A successful test is reported as:

```text
******************************
* Testcase '...' passed
******************************
```

At the end, the script prints an overall report with the number of attempted and passed tests.

## Running a Test Manually

For example:

```bash
gcc -g -std=gnu99 \
    sim4.c \
    sim4_test_commonCode.c \
    test_20_ADD_SUB_ADDI.c \
    -lm \
    -o test_add
```

Then:

```bash
./test_add
```

## Instruction Formats

### R-Type

```text
 31       26 25    21 20    16 15    11 10     6 5       0
+-----------+--------+--------+--------+--------+-----------+
|  opcode   |   rs   |   rt   |   rd   | shamt  |  funct   |
+-----------+--------+--------+--------+--------+-----------+
```

Examples:

```text
ADD
SUB
AND
OR
XOR
SLT
```

### I-Type

```text
 31       26 25    21 20    16 15                     0
+-----------+--------+--------+-------------------------+
|  opcode   |   rs   |   rt   |       immediate         |
+-----------+--------+--------+-------------------------+
```

Examples:

```text
ADDI
ADDIU
SLTI
BEQ
LW
SW
```

### J-Type

```text
 31       26 25                                      0
+-----------+------------------------------------------+
|  opcode   |                 address                  |
+-----------+------------------------------------------+
```

Example:

```text
J
```

## Testing Strategy

The repository tests the simulator in progressively larger pieces.

Examples include:

- instruction fetching
- control-signal generation
- ALU inputs
- ALU execution
- memory execution
- complete instructions
- jump address behavior
- invalid instructions
- syscalls
- sequences of multiple instructions
- arithmetic and logical operations

This stage-by-stage structure is useful for debugging because a failure can usually be isolated to one part of the CPU datapath.

## Educational Purpose

This project demonstrates how the datapath of a simplified MIPS processor works at the software level.

Instead of executing MIPS instructions directly on hardware, the simulator represents hardware components with C structures and functions. This makes concepts such as instruction decoding, control signals, ALU behavior, memory access, branches, jumps, and register write-back easier to inspect and test.

## Author

**Ali Elbekov**

CSc 252 — SIM4

## License

No license is currently included in this repository. All rights are reserved by the repository owner unless a license is added.
