#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

void
sys_join(void)
{
    // // Hector's functions
    // void join_files(char file1_lines[][256], int count1, char file2_lines[][256], int count2);
    // void print_joined_line(char* field, char* rest1, char* rest2);
    // char* get_rest_of_line(char* line);
    // 
    // 
    // // Demetrius's functions
    // char* get_first_field(char* line);
    // int read_file_lines(char* filename, char lines[][256], int count2);
    
    /**
     * Extracts the first field (word) from a line
     * Returns a pointer to the first field, or NULL if line is empty
     */
    char* get_first_field(char* line) {
        if (line == NULL || *line == '\0') {
            return NULL;
        }
        
        // Skip leading whitespace
        while (*line == ' ' || *line == '\t') {
            line++;
        }
        
        // If line is empty after skipping whitespace
        if (*line == '\0') {
            return NULL;
        }
        
        return line;
    }

    /**
     * Function to get the rest of the line so it is everything 
     * after the first word 
    */
    char* get_rest_of_line(char* line){

        // return empty if line is not valid
        if(line == NULL || *line == '\0'){
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

    /**
     * Reads lines from a file into the provided array
     * Returns the number of lines successfully read
     */
    int read_file_lines(char* filename, char lines[][256], int max_count) {
        int fd = open(filename, 0); // 0 = O_RDONLY
        if (fd < 0) {
            printf("Error: Cannot open file '%s'\n", filename);
            return 0;
        }
        
        int line_count = 0;
        char buffer[256];
        int pos = 0;
        char c;
        
        while (line_count < max_count && read(fd, &c, 1) > 0) {
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
        
        close(fd);
        return line_count;
    }
    /**
     * function prints joined line 
     * "first word" "rest line file 1" "rest line file 2"
    */
    void print_joined_line(char* field, char* rest1, char* rest2){
        printf("%s %s %s\n", field, rest1, rest2);
    }

    /**
     * Compares lines from both files and looks for matches in the 
     * first field
     * If the fields match then print joined line
    */
    void join_files(char file1_lines[][256], int count1, char file2_lines[][256], int count2){

        // iterate through each line from file 1 
        for (int i =0; i< count1; i++){
            char* field1 = get_first_field(field1_lines[i]); // reads full line but only gets first word to check for match
            // skip is field is empty 
            if(field1 == 0){
                continue;
            }


            // iterate through lines in file two 
            for (int j =0; j < count2; j++){
                char* field2 = get_first_field(field2_lines[j]); // read full line but only gets first word
                // skip if field is empty
                if(field2 == 0){
                    continue;
                }

                // check if both words are equal returns 0 if true
                if(strcmp(field1, field2) == 0){
                    // get rest of lines for both files from where we got field
                    char* rest1 = get_rest_of_line(file1_lines[i]);
                    char* rest2 = get_rest_of_line(file2_lines[j]);

                    print_joined_line(field1, rest1, rest2);


                }
            }
        }
        printf("join_files() completed");
    }
}
