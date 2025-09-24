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

    int compare_strings(char* str1, char* str2){
    	while (*str1 && *str2){
    		if (*str1 != *str2){
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
    char* get_rest_of_line(char* line){

        // return empty if line is not valid
        if(line == 0 || *line == '\0'){
            return "";
        }

        char* ptr = line;
        // skip leading whitespace
        while(*ptr ==  ' ' || *ptr == '\t'){
            ptr++;
        }
        // skip first field until reached whitespace or end of line
        while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' ){
            ptr++;
        }
        // skip whitespace between first field and rest
        while(*ptr == ' ' || *ptr == '\t'){
            ptr++;
        }
        return ptr;
    }

    void terminate_first_field(char* line){
    	char* ptr = line;

		// skip whitespace
    	while (*ptr == ' ' || *ptr == '\t'){
    		ptr++;
    	}

    	// get the end of the first word
    	while (*ptr != '\0' && *ptr != ' ' && *ptr != '\n' && *ptr != '\t'){
    		ptr++;
    	}
    	// terminate first word
    	if(*ptr != '\0'){
    		*ptr = '\0';
    	}
    }

    /**
     * Reads lines from a file into the provided array
     * Returns the number of lines successfully read
     */
    int read_file_lines(char* filename, char lines[][256], int max_count) {
        int file = open(filename, 0); // 0 = O_RDONLY
        if (file < 0) {
            printf("Error: Cannot open file '%s'\n", filename);
            return 0;
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
        void print_joined_line(int output_file, char* field, char* rest1, char* rest2){
            printf("%s %s %s\n", field, rest1, rest2);

            // Write to the file
            write(output_file, field, strlen(field));
            write(output_file, " ", 1);
            write(output_file, rest1, strlen(rest1));
            write(output_file, " ", 1);
            write(output_file, rest2, strlen(rest2));
            write(output_file, "\n", 1);
        }
    
    /**
     * Compares lines from both files and looks for matches in the 
     * first field
     * If the fields match then print joined line
    */
    void join_files(char file1_lines[][256], int count1, char file2_lines[][256], int count2, const char* output_file){
        int matches_found = 0;

        // Open the output file for writing (create it if it doesn't exist)
        int output_fd = open(output_file, O_CREATE | O_RDWR);
        if (output_fd < 0) {
            printf("Error: Cannot open output file '%s'\n", output_file);
            return;
        }

        // iterate through each line from file 1 
        for (int i =0; i< count1; i++){
            char line1_copy[256]; // make copy to avoid messing with original
            int k = 0;
            while (k<255 && file1_lines[i][k] != '\0'){
                line1_copy[k] = file1_lines[i][k];
                k++;
            }
            line1_copy[k] = '\0';
            
            char* field1 = get_first_field(line1_copy); // reads full line but only gets first word to check for match
            // skip is field is empty 
            if(field1 == 0){
                continue;
            }

            terminate_first_field(field1); // get rid of first word to make comparing easier

            // iterate through lines in file two 
            for (int j =0; j < count2; j++){
                char line2_copy[256]; // make copy to avoid messing with original
                int m = 0;
                while (m<255 && file2_lines[j][m] != '\0'){
                    line2_copy[m] = file2_lines[j][m];
                    m++;
                }
                line2_copy[m] = '\0';
                            
                char* field2 = get_first_field(line2_copy); // read full line but only gets first word
                // skip if field is empty
                if(field2 == 0){
                    continue;
                }
                terminate_first_field(field2);

                // check if both words are equal returns 0 if true
                if(compare_strings(field1, field2) == 0){
                    matches_found++;
                
                    // get rest of lines for both files from where we got field
                    char* rest1 = get_rest_of_line(file1_lines[i]);
                    char* rest2 = get_rest_of_line(file2_lines[j]);

                    print_joined_line(output_fd, field1, rest1, rest2);

                }
            }                
        }
        if(matches_found == 0){
            printf("No matches found\n");
        }
        // Close the output file
        close(output_fd);
    }

int main(int argc, char *argv[]){
    // ensure correct usage of command line args
    if (argc != 3){
        printf("Usage: join file1.txt file2.txt\n");
        exit(1);
    }

    printf("Joining files '%s' and '%s'...\n", argv[1], argv[2]);


    // read both files
    printf("Reading first file...");
    int count1 = read_file_lines(argv[1], file1_lines, 50);
    printf("Read %d lines from file1\n", count1);
    if(count1 == 0){
    	printf("ERROR: failed to read lines from %s\n", argv[1]);
    	exit(1);
    }

    printf("Reading second file...");
    int count2 = read_file_lines(argv[2], file2_lines, 50);
    printf("Read %d lines from file2\n", count2);
        if(count2 == 0){
    	printf("ERROR: failed to read lines from %s\n", argv[2]);
    	exit(1);
    }
    // join both files
    printf("joining both files\n");
    join_files(file1_lines, count1, file2_lines, count2, "joined.txt");
    printf("saving in a new file called joined.txt\n");
    printf("join completed\n");

    exit(0);
}
