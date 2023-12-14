//Sivani Kommineni and Ankita Panchal

void printString(char* str);
void printChar(char c);
void readFile(char* fileName, char* buffer, int* bytesRead);
void readString(char* str);
void readSector(char* buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void printNewLine();
void executeProgram(char* name);
void terminate();

void main() {
    char inputLine[80];
    char sectorBuffer[512];
    char buffer[13312];   /*Maximum size of a file*/
    int sectorsRead;

     makeInterrupt21();
     
     interrupt(0x21, 3, "messag", buffer, &sectorsRead);   /*read the file into buffer*/ 
     if (sectorsRead>0)
     interrupt(0x21, 0, buffer, 0, 0);   /*print out the file*/ 
     else
     interrupt(0x21, 4, "shell");
     
     while(1);   /*hang up*/ 
}

void terminate(){
     char shellname[6]; 
     shellname[0] ='s';
     shellname[1]='h'; 
     shellname[2]='e';
     shellname[3]='l'; 
     shellname[4]='l'; 
     shellname[5]='\0';  
     executeProgram(shellname); 
}

void readFile(char* fileName, char* buffer, int* bytesRead) {
    char directory[512];

    int fileEntrySize=32;  // Each file entry in the directory is 32 bytes
    int directorySize = 512;  // The directory sector is 512 bytes
    int fileEntry =0;
    int found = 0;
    int i, sector;
    readSector(directory, 2);  // Read the directory sector 

    for (fileEntry = 0; fileEntry < directorySize; fileEntry += fileEntrySize) {
        for (i = 0; i < 6; i++) {
            if (fileName[i] != directory[fileEntry + i]) {
                break;
            }
        }
        if (i == 6) {
            found = 1;
            break;  // File name matched, break out of the loop
        }
    }

    if (!found) {
        *bytesRead = 0;  // File not found, set the number of sectors read to 0
        return;
    }

    // File found, start loading the file sector by sector
    sector = directory[fileEntry + 6];
    *bytesRead = 0;
    while (sector != 0) {
        readSector(buffer, sector);  // Read the sector into the buffer
        buffer += 512;  // Increment the buffer address
        sector = directory[fileEntry + 7];  // Move to the next sector
        (*bytesRead)++;
    }
}

void printString(char* str) {
    while (*str != '\0') {
        printChar(*str);
        str += 0x1;
    }
}

void printChar(char c) {
    int ah, al, ax;
    ah = 0xe;
    al = c;
    ax = (ah * 256) + al;
    interrupt(0x10, ax, 0, 0, 0);
}

void readString(char* str) {
    char* start;
    char c;
    while (c != 0xd) {
        c = interrupt(0x16, 0, 0, 0, 0);
        if (c == 0x8) {
            if (str != start) {
                printChar(0x8);
                printChar(0x20);
                printChar(0x8);
                str -= 0x1;
                *str = 0x20;
            }
        } else {
            printChar(c);
            *str = c;
            str += 0x1;
        }
    }
    printChar(c);
    printChar(0xa);

    *str = 0xa;
    str += 0x1;
    *str = 0x0;
}

void readSector(char* buffer, int sector) {
    int ah, al, bx, ch, cl, dh, dl, ax, cx, dx;
    ah = 2;
    al = 1;
    bx = buffer;
    ch = 0; // track number
    cl = sector + 1; // relative sector number
    dh = 0; // head number
    dl = 0x80;

    ax = (ah * 256) + al;
    cx = (ch * 256) + cl;
    dx = (dh * 256) + dl;
    buffer = interrupt(0x13, ax, bx, cx, dx);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
    if (ax == 0) {
        printString(bx);
    } else if (ax == 1) {
        readString(bx);
    } else if (ax == 2) {
        readSector(bx, cx);
    } else if (ax == 3) {
        readFile(bx,cx,dx );
    }else if(ax == 4){
        executeProgram(bx);        
    }else if (ax == 5){
        terminate();
    } else{
        printString("Invalid ax");
    }
}


void executeProgram(char* name) {
    char buffer[512 * 10];  
    int segment = 0x2000;
    int offset = 0;

    //Call readFile to load the file into a buffer
    int sectorsRead,i;
    readFile(name, buffer, &sectorsRead);

    if (sectorsRead == 0) {
        printString("Program not found.\n");
        return;
    }

    //Transfer the file from the buffer into memory at segment 0x2000

    for (i = 0; i < 512 * sectorsRead; ++i) {
        putInMemory(0x2000, offset, buffer[i]);
        ++offset;
        if (offset == 0x1000) {
            // Move to the next segment
            offset = 0;
            ++segment;
        }
    }

  
    launchProgram(0x2000);
   
}

