#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"


// // Hector's functions
//void join_files(char file1_lines[][256], int count1, char file2_lines[][256], int count2, const char* output_file));
//void print_joined_line(int output_file, char* field, char* rest1, char* rest2);
//char* get_rest_of_line(char* line);
// 
// 
// // Demetrius's functions
//char* get_first_field(char* line);
// int read_file_lines(char* filename, char lines[][256], int count2);

// helper functions 
//compare_strings(char* str1, char* str2)
//terminate_first_field(char* line)

/**
 * String comparison iterates through each char in both
 * strings to see if they match if non return non-zero
 */

static char file1_lines[50][256];
static char file2_lines[50][256];

int compare_strings(char* str1, char* str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

/**
 * Extracts the first field (word) from a line
 * Returns a pointer to the first field, or NULL if line is empty
 */

char* get_first_field(char* line) {
    if (line == 0 || *line == '\0') {
        return 0;
    }
    
    // Skip leading whitespace
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    
    // If line is empty after skipping whitespace
    if (*line == '\0') {
        return 0;
    }
    
    return line;
}

/**
 * Function to get the rest of the line so it is everything 
 * after the first word 
*/
char* get_rest_of_line(char* line) {

    // return empty if line is not valid
    if(line == 0 || *line == '\0') {
        return "";
    }

    char* ptr = line;
    // skip leading whitespace
    while(*ptr ==  ' ' || *ptr == '\t') {
        ptr++;
    }
    // skip first field until reached whitespace or end of line
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' ) {
        ptr++;
    }
    // skip whitespace between first field and rest
    while(*ptr == ' ' || *ptr == '\t') {
        ptr++;
    }
    return ptr;
}

void terminate_first_field(char* line) {
    char* ptr = line;

    // skip whitespace
    while (*ptr == ' ' || *ptr == '\t') {
        ptr++;
    }

    // get the end of the first word
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\n' && *ptr != '\t') {
        ptr++;
    }
    // terminate first word
    if(*ptr != '\0') {
        *ptr = '\0';
    }
}

/**
 * Reads lines from a file into the provided array
 * Returns the number of lines successfully read
 * Returns 0 if file cannot be opened
 * Returns -1 if metadata cannot be retrieved
 * Returns -2 if file is empty
 */
int read_file_lines(char* filename, char lines[][256], int max_count) {
    int file = open(filename, 0); // 0 = O_RDONLY
    struct stat st; // stat struct contains a member called size, which holds the file's size in bytes
    
    if (file < 0) {
        close(file);
        return 0;
    }

    if (fstat(file, &st) < 0) { //check metadata
        close(file);
        return -1;
    }

    if (st.size == 0) { //check size to see if empty txt or not, join does not work if any or both files empty
        close(file);
        return -2;
    }
    
    int line_count = 0;
    char buffer[256];
    int pos = 0;
    char c;
    
    while (line_count < max_count && read(file, &c, 1) > 0) {
        if (c == '\n') {
            // End of line found
            buffer[pos] = '\0';
            
            // Copy the line to the array
            int i = 0;
            while (i < 255 && buffer[i] != '\0') {
                lines[line_count][i] = buffer[i];
                i++;
            }
            lines[line_count][i] = '\0';
            
            line_count++;
            pos = 0; // Reset position for next line
        } else if (pos < 255) {
            // Add character to buffer
            buffer[pos] = c;
            pos++;
        }
    }
 
    // Handle the last line if file doesn't end with newline
    if (pos > 0 && line_count < max_count) {
        buffer[pos] = '\0';
        int i = 0;
        while (i < 255 && buffer[i] != '\0') {
            lines[line_count][i] = buffer[i];
            i++;
        }
        lines[line_count][i] = '\0';
        line_count++;
    }
    
    close(file);
    return line_count;
}

/**
 * function prints joined line 
 * "first word" "rest line file 1" "rest line file 2"
*/
void print_joined_line(int output_file, char* field, char* rest1, char* rest2) {
    // Only print to console if NOT writing to a file
    if (output_file < 0) {
        printf("%s %s %s\n", field, rest1, rest2);
    }
    
    // check if there is a valid file descriptor
    if (output_file >= 0) {
        // Write to the file 
        write(output_file, field, strlen(field));
        write(output_file, " ", 1);
        write(output_file, rest1, strlen(rest1));
        write(output_file, " ", 1);
        write(output_file, rest2, strlen(rest2));
        write(output_file, "\n", 1);
    }
}

/**
 * Compares lines from both files and looks for matches in the 
 * first field
 * If the fields match then print joined line
*/
void join_files(char file1_lines[][256], int count1, char file2_lines[][256], int count2, const char* output_file) {
    int matches_found = 0;      // count for amount of matches found
    int output_fd = -1;         // value of file descriptor of output file (-1 no valid file)

    // Open the output file for writing (create it if it doesn't exist)
    if(output_file != 0) {
        // changes the value of output_fd to indicate we have a valid file
        output_fd = open(output_file, O_CREATE | O_WRONLY | O_TRUNC);  // open file with flags (create new file if it does not exist | 
                                                                       //  write access only | empty file if already exists)
        if (output_fd < 0) {
            printf("Error: Cannot open output file '%s'\n", output_file);
            return;
        }
    }

    // iterate through each line from file 1 
    for (int i = 0; i < count1; i++) {
        char line1_copy[256]; // make copy to avoid messing with original
        int k = 0;

        // string copy
        while (k < 255 && file1_lines[i][k] != '\0') {
            line1_copy[k] = file1_lines[i][k];
            k++;
        }
        line1_copy[k] = '\0';
        
        char* field1 = get_first_field(line1_copy); // reads full line but only gets first word to check for match
        // skip is field is empty 
        if(field1 == 0) {
            continue;
        }

        terminate_first_field(field1); // get rid of first word to make comparing easier

        // iterate through lines in file two 
        for (int j = 0; j < count2; j++) {
            char line2_copy[256]; // make copy to avoid messing with original
            int m = 0;

            // string copy
            while (m < 255 && file2_lines[j][m] != '\0') {
                line2_copy[m] = file2_lines[j][m];
                m++;
            }
            line2_copy[m] = '\0';
                        
            char* field2 = get_first_field(line2_copy); // read full line but only gets first word
            // skip if field is empty
            if(field2 == 0) {
                continue;
            }
            terminate_first_field(field2); // get rid of first word to make comparing easier

            // check if both words are equal returns 0 if true
            if(compare_strings(field1, field2) == 0) {
                matches_found++;
            
                // get rest of lines for both files from where we got field
                char* rest1 = get_rest_of_line(file1_lines[i]);
                char* rest2 = get_rest_of_line(file2_lines[j]);

                // print using format
                print_joined_line(output_fd, field1, rest1, rest2);
            }
        }                
    }
    
    // print message if no matches were found
    if(matches_found == 0) {
        printf("No matches found\n");
    }
    
    // Close the output file
    close(output_fd);
}

void print_usage() {
    printf("Usage: join file1.txt file2.txt [outputfile.txt]\n");
    printf("\nDescription:\n");
    printf("  Joins two files based on matching first fields in each line.\n");
    printf("  Output format: <matching_field> <rest_of_line1> <rest_of_line2>\n");
    printf("\nArguments:\n");
    printf("  file1.txt       First input file\n");
    printf("  file2.txt       Second input file\n");
    printf("  outputfile.txt  (Optional) Output file for results\n");
    printf("\nOptions:\n");
    printf("  --help          Display this help message\n");
}

int main(int argc, char *argv[]) {
    // Handle --help flag
    if (argc == 2 && compare_strings(argv[1], "--help") == 0) {
        print_usage();
        exit(0);
    }

    char* output_file = 0;
    
    // Validate command line arguments
    if (argc == 4) {
        output_file = argv[3];
    } else if (argc != 3) {
        print_usage();
        exit(1);
    }

    // Read both files
    int count1 = read_file_lines(argv[1], file1_lines, 50);
    if(count1 <= 0) {
        if(count1 == 0) {
            printf("ERROR: failed to open %s\n", argv[1]);
        } else if(count1 == -1) {
            printf("ERROR: failed to retrieve metadata from %s\n", argv[1]);
        } else if(count1 == -2) {
            printf("ERROR: %s is empty\n", argv[1]);
        }
        print_usage();
        exit(1);
    }
    
    int count2 = read_file_lines(argv[2], file2_lines, 50);
    if(count2 <= 0) {
        if(count2 == 0) {
            printf("ERROR: failed to open %s\n", argv[2]);
        } else if(count2 == -1) {
            printf("ERROR: failed to retrieve metadata from %s\n", argv[2]);
        } else if(count2 == -2) {
            printf("ERROR: %s is empty\n", argv[2]);
        }
        print_usage();
        exit(1);
    }
    
    // Join both files
    join_files(file1_lines, count1, file2_lines, count2, output_file);
    if(output_file != 0) {
        printf("Output saved to %s\n", output_file);
    }
    
    printf("join completed\n");
    
    exit(0);
}
