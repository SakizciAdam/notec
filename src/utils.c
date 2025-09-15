#include "utils.h"
#include <stdio.h>


bool readOnly = 0;
char* fileName = NULL;
bool fileSet = 0;


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


void initColors() {
    if (!has_colors()) return;
    start_color();
    use_default_colors();
    init_color(COLOR_RED, 700, 0, 0);



    init_pair(CP_DEFAULT,   COLOR_WHITE,   -1);
    init_pair(CP_COMMENT,   COLOR_CYAN,    -1);   // FOREGROUND_GREEN | FOREGROUND_BLUE
    init_pair(CP_STRING,    COLOR_GREEN,   -1);   // FOREGROUND_GREEN
    init_pair(CP_CHAR,      COLOR_MAGENTA, -1);   // FOREGROUND_GREEN | FOREGROUND_RED -> magenta
    init_pair(CP_BRACKET,   COLOR_YELLOW,  -1);   // FOREGROUND_RED | FOREGROUND_GREEN -> yellow
    init_pair(CP_NUMBER,    COLOR_YELLOW,  -1);   // same as bracket
    init_pair(CP_PREPROC,   COLOR_MAGENTA, -1);   // FOREGROUND_RED | FOREGROUND_BLUE -> magenta
    init_pair(CP_KEYWORD,   COLOR_MAGENTA,    -1);   // FOREGROUND_BLUE | FOREGROUND_GREEN -> cyan
    init_pair(CP_FUNCTION,  COLOR_MAGENTA, -1);   // FOREGROUND_RED | FOREGROUND_BLUE -> magenta-ish
    init_pair(CP_SELECTION, COLOR_WHITE,   COLOR_BLUE); // selection: blue background
}