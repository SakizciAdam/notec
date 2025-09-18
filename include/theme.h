#ifndef THEME_H
#define THEME_H

#include <string.h>
#ifdef _WIN32
    #include <windows.h>
    #include <curses.h>
    #include <io.h>

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

#endif


enum {
    CP_DEFAULT = 1,
    CP_KEYWORD=2,
    CP_STRING=3,
    CP_CHAR=4,
    CP_NUMBER=5,
    CP_COMMENT=6,
    CP_FUNCTION=7,
    CP_PAREN=8,
    CP_INCLUDE=9,
    CP_SELECTION=11,
    CP_STATUS=10
};

void set_default_theme();

int hex_to_rgb(const char *hex, short *r, short *g, short *b);

void set_color(short id,const char *hex);

void set_color_bg(short id,const char *fg,const char *bg);

int load_theme(const char *filename);

void init_colors();


#endif