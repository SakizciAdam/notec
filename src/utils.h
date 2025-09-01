#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <conio.h>
    #define RETURN 13
    #define BACKSPACE 8
    #define UP_ARROW 72
    #define LEFT_ARROW 75
    #define RIGHT_ARROW 77
    #define DOWN_ARROW 80
#else
    #include <ctype.h>
    #include <string.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <termios.h>  
    #include <ncurses.h>
    #include <sys/ioctl.h>
    
    #define RETURN 10
    #define BACKSPACE 7
    #define UP_ARROW 3
    #define LEFT_ARROW 4
    #define RIGHT_ARROW 5
    #define DOWN_ARROW 2
    bool kbhit();
#endif

void cls();

extern bool readOnly;
extern char* fileName;
extern bool fileSet;

#endif // UTILS_H
