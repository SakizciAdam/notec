#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#include <string.h>
#include "theme.h"


#define WRITING_MODE 0
#define CONTROL_MODE 1




#ifdef _WIN32
    #include <windows.h>
    #include <curses.h>
    #include <io.h>
    #define RETURN 13
    #define BACKSPACE 8
    #define UP_ARROW 72
    #define LEFT_ARROW 75
    #define RIGHT_ARROW 77
    #define DOWN_ARROW 80
#else
    #include <stdbool.h>
    #include <ctype.h>
    
    #include <stdlib.h>
    #include <unistd.h>
    #include <termios.h>  
    #include <ncurses.h>
    #include <sys/ioctl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    
    #define RETURN 10
    #define BACKSPACE 7
    #define UP_ARROW 3
    #define LEFT_ARROW 4
    #define RIGHT_ARROW 5
    #define DOWN_ARROW 2

    

    bool kbhit();
#endif



int regexFind(const char *text, const char *pattern, int *start, int *len);
extern bool readOnly;
extern char* fileName;
extern bool fileSet;
extern int mode;

extern char* text;
extern int length;
extern int cursorX;
extern int cursorY;
extern int scrollX;
extern int scrollY;
extern int selStart;
extern int selEnd;
extern int arrow;
extern char* statusText;
extern int statusLength;


#endif // UTILS_H
