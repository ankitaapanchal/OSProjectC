//Sivani Kommineni and Ankita Panchal

int compareStrings(char *command, char *input);
#define Buffer_size 13312

void main() {
        while(1) {
                char input[Buffer_size];
                char buffer[Buffer_size];
                int sectorsRead, i;

                for(i=0; i<512; i++) {
                        input[i] = '\0';
                        buffer[i] = '\0';
                }

                syscall(0, "A:>");
                syscall(1, input);

                if(isCommand("type", input)) {
                        syscall(3, input+5, buffer, &sectorsRead);
                        if(sectorsRead <= 0) 
                            {
                             syscall(0, "Invalid file\r\n");
                             continue;
                            }
                        syscall(0, buffer);
                        syscall(0, "\r\n");
                } else if(isCommand("exec", input)) {
                        syscall(3, input+5, buffer, &sectorsRead);
                        if(sectorsRead<=0) syscall(0, "Invalid file\r\n");
                        else {
                                syscall(4, input+5);
                        }
                } else {
                        syscall(0, "Command not recognized\r\n");
                }
     }
}	
int isCommand(char *command, char *input) {
    while (*command != '\0' && *command != ' ' && *input != '\0' && *input != ' ') {
        if (*command != *input) {
            return 0; // false
        }
        command++;
        input++;
     }

     return 1; // true
}

