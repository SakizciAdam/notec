#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#define CP_DEFAULT    1
#define CP_COMMENT    2
#define CP_STRING     3
#define CP_CHAR       4
#define CP_BRACKET    5
#define CP_NUMBER     5
#define CP_PREPROC    6
#define CP_KEYWORD    7
#define CP_FUNCTION   8
#define CP_SELECTION  9

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
int regexFind(const char *text, const char *pattern, int *start, int *len);
extern bool readOnly;
extern char* fileName;
extern bool fileSet;

void initColors();

#endif // UTILS_H
