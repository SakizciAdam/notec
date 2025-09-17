#include "utils.h"



bool readOnly = 0;
char* fileName = NULL;
bool fileSet = 0;
int mode=0;

char* text = NULL;
int length = 0;
int cursorX = 0;
int cursorY = 0;
int scrollX = 0;
int scrollY = 0;
int selStart = -1;
int selEnd = -1;
int arrow=0;
char* statusText;
int statusLength=0;


void cls() {
    printf("\033[2J\033[H");
    fflush(stdout);

}
#ifndef _WIN32
bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}




#endif


void init_colors() {
    start_color();
    use_default_colors();
    init_pair(CP_DEFAULT,   COLOR_WHITE,  -1);
    init_pair(CP_KEYWORD,   COLOR_CYAN,   -1);
    init_pair(CP_STRING,    COLOR_GREEN,  -1);
    init_pair(CP_CHAR,      COLOR_MAGENTA,-1);
    init_pair(CP_NUMBER,    COLOR_YELLOW, -1);
    init_pair(CP_COMMENT,   COLOR_BLUE,   -1);
    init_pair(CP_FUNCTION,  COLOR_RED,    -1);
    init_pair(CP_PAREN,     COLOR_YELLOW, -1);
    init_pair(CP_INCLUDE,   COLOR_MAGENTA,-1);
    init_pair(CP_SELECTION, COLOR_BLACK,  COLOR_WHITE);
    init_pair(CP_STATUS,    COLOR_BLACK,  COLOR_WHITE);
}
