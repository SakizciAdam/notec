#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <conio.h>
#else
    #include <ctype.h>
    #include <string.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <termios.h>  
    #include <ncurses.h>
    #include <sys/ioctl.h>

    bool kbhit();
#endif

void cls();

extern bool readOnly;
extern char* fileName;
extern bool fileSet;

#endif // UTILS_H
