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
struct RGB {
  int r,g,b;

};
struct RGB hex_to_rgb(int hex) {
    struct RGB color;
    color.r = (hex >> 16) & 0xff;
    color.g = (hex >> 8) & 0xff;
    color.b = hex & 0xff;

    return color;
}
short rgb_to_ncurses(int val) {
    return (val * 1000) / 255;
}
void setColor(int idx, int hex) {


    if (can_change_color()) {
        struct RGB color = hex_to_rgb(hex);
        init_color(idx, rgb_to_ncurses(color.r), rgb_to_ncurses(color.g), rgb_to_ncurses(color.b));
    } else {
        printf("Failed to change color");
        exit(0);
    }
}

void initColors() {
    if (!has_colors()) return;
    start_color();


    setColor(CP_DEFAULT,  0xd0d0d0 );
    setColor(CP_COMMENT,  0x808080 );   // FOREGROUND_GREEN | FOREGROUND_BLUE
    setColor(CP_STRING,   0x98fb98);   // FOREGROUND_GREEN
    setColor(CP_CHAR,     0x98fb98 );   // FOREGROUND_GREEN | FOREGROUND_RED -> magenta
    setColor(CP_BRACKET,  0x7fffd4 );   // FOREGROUND_RED | FOREGROUND_GREEN -> yellow
    setColor(CP_NUMBER,    0xf0e68c);   // same as bracket
    setColor(CP_PREPROC,  0x7fffd4 );   // FOREGROUND_RED | FOREGROUND_BLUE -> magenta
    setColor(CP_KEYWORD,   0xff5555);   // FOREGROUND_BLUE | FOREGROUND_GREEN -> cyan
    setColor(CP_FUNCTION,0xdda0dd) ; 




    init_pair(CP_DEFAULT,   CP_DEFAULT,   -1);
    init_pair(CP_COMMENT,   CP_COMMENT,    -1);   // FOREGROUND_GREEN | FOREGROUND_BLUE
    init_pair(CP_STRING,    CP_STRING,   -1);   // FOREGROUND_GREEN
    init_pair(CP_CHAR,      CP_CHAR, -1);   // FOREGROUND_GREEN | FOREGROUND_RED -> magenta
    init_pair(CP_BRACKET,   CP_BRACKET,  -1);   // FOREGROUND_RED | FOREGROUND_GREEN -> yellow
    init_pair(CP_NUMBER,    CP_NUMBER,  -1);   // same as bracket
    init_pair(CP_PREPROC,   CP_PREPROC, -1);   // FOREGROUND_RED | FOREGROUND_BLUE -> magenta
    init_pair(CP_KEYWORD,   CP_KEYWORD,    -1);   // FOREGROUND_BLUE | FOREGROUND_GREEN -> cyan
    init_pair(CP_FUNCTION,  CP_FUNCTION, -1);   // FOREGROUND_RED | FOREGROUND_BLUE -> magenta-ish
    init_pair(CP_SELECTION, CP_DEFAULT,   CP_DEFAULT); // selection: blue background
}