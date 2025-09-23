#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    // ensure correct usage of command line args
    if (argc != 3){
        printf("Usage: join file1.txt file2.txt\n");
        exit(1);
    }

    printf("Joining files '%s' and '%s'...\n", argv[1], argv[2]);


    // initiate data structure for file content
    // set two arrays of 50 lines and 256 chars each
    char file1_lines[50][256];
    char file2_lines[50][256];


    // read both files
    printf("Reading first file...");
    int count1 = join.read_file_lines(argv[1], file1_lines, 50);

    printf("Reading second file...");
    int count2 = join.read_file_lines(argv[2], file2_lines, 50);
    
    // join both files
    printf("joining both files");
    join.join_files(file1_lines, count1, file2_lines, count2);

    exit(0);
}
