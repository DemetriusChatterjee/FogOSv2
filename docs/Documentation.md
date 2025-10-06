# Software Documentation

## Building the Software

### Prerequisites
- RISC-V toolchain (riscv64-unknown-elf-gcc or riscv64-linux-gnu-gcc)
- QEMU RISC-V emulator (version 7.2 or higher)
- Make build system

### Build Steps
```bash
make
```
This will:
- Compile the kernel from source files in the `kernel/` directory
- Compile user programs from the `user/` directory
- Copy test data files from `testData/` to the root directory
- Create the filesystem image (`fs.img`) containing all programs and data files

## Running the Software

### Start QEMU
```bash
make qemu
```
This launches the RISC-V emulator with the compiled kernel and filesystem.

### Exit QEMU
Press `Ctrl+A` then `X` to quit the emulator.

**Important:** After exiting QEMU, run `make clean` to remove build artifacts and copied test files.

### Run with automatic cleanup
```bash
make qemu-clean
```
This runs QEMU and automatically cleans build artifacts after you exit.

## Testing the Software

### Understanding the join command
The `join` command merges two text files based on matching first fields (words) in each line. For each match found, it outputs a combined line in the format:
```
<matching_field> <rest_of_line_from_file1> <rest_of_line_from_file2>
```

### Access test data files
Once in QEMU, the test data files are available in the filesystem:
- `file1.txt`, `file2.txt` - Sample department data (25 lines each)
- `MC_Item.txt`, `MC_Price.txt` - Inventory data
- `students.txt`, `grades.txt` - Student records
- `products.txt`, `prices.txt` - Product information
- `Fog_Emp.txt`, `Fog_Perf.txt` - Employee data
- `empty.txt` - Empty test file

### Run join command

**Basic usage (output to console):**
```bash
join file1.txt file2.txt
```
Expected output: 25 joined lines matching names from both files, displaying department info with grades.

**With output file:**
```bash
join file1.txt file2.txt output.txt
```
This saves the joined results to `output.txt` while also displaying them on console.

**Get help:**
```bash
join --help
```

**Example output format:**
When joining students.txt and grades.txt, each matching line combines:
- The matching first field (e.g., "001")
- The remainder of the line from students.txt (e.g., "Alice Engineering Robotics")
- The remainder of the line from grades.txt (e.g., "A+ Excellent Outstanding")

```
001 Alice Engineering Robotics A+ Excellent Outstanding
002 Bob Physics Quantum B+ Good Solid
003 Charlie Chemistry Polymers A- Very Strong
005 Ethan Biology Genetics B Fair Average
join completed
...
```

The output shows 4 total matches (one for each person in both files).

## Cleaning Up

### Remove all build artifacts and copied test files
```bash
make clean
```
This removes:
- Compiled object files (*.o)
- Kernel binary and filesystem image
- Temporary files
- Test data files copied to root (preserves originals in `testData/`)
- Note: `compile_flags.txt` in root is preserved

## Project Structure

- `kernel/` - Operating system kernel source code
- `user/` - User-space programs and utilities
- `testData/` - Original test data files (preserved during clean)
- `mkfs/` - Filesystem creation utility
- `Makefile` - Build configuration and targets
