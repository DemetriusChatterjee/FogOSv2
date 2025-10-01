# Software Documentation

## Building the Software

### Build Steps
```bash
make
```
This will:
- Compile the kernel from source files in the `kernel/` directory
- Compile user programs from the `user/` directory
- Copy test data files from `tests/` to the root directory
- Create the filesystem image (`fs.img`) containing all programs and data files

## Running the Software

### Start QEMU
```bash
make qemu
```
This launches the RISC-V emulator with the compiled kernel and filesystem.

### Exit QEMU
Press `Ctrl+A` then `X` to quit the emulator.

### Run with automatic cleanup [OPTIONAL]
```bash
make qemu-clean
```
This runs QEMU and automatically cleans build artifacts after you exit.

## Testing the Software

### Access test data files
Once in QEMU, the test data files are available in the filesystem:
- `file1.txt`, `file2.txt` - Sample department data
- `MC_Item.txt`, `MC_Price.txt` - Inventory data
- `students.txt`, `grades.txt` - Student records
- `products.txt`, `prices.txt` - Product information
- `Fog_Emp.txt`, `Fog_Perf.txt` - Employee data
- `empty.txt` - Empty test file

### Run join command
You can test the join functionality with two input files:
```bash
join file1.txt file2.txt
```

You can also specify an output file:
```bash
join file1.txt file2.txt output.txt
```

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
- `tests/` - Original test data files (preserved during clean)
- `mkfs/` - Filesystem creation utility
- `Makefile` - Build configuration and targets
