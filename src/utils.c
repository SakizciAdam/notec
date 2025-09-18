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
int hex_to_rgb(const char *hex, short *r, short *g, short *b) {
    if (hex[0] == '#') hex++;
    unsigned int val = strtol(hex, NULL, 16);
    *r = ((val >> 16) & 0xFF) * 1000 / 255;
    *g = ((val >> 8) & 0xFF) * 1000 / 255;
    *b = (val & 0xFF) * 1000 / 255;
    return 0;
}

void set_color(short id,const char *hex){
    short r,g,b;
    hex_to_rgb(hex,&r,&g,&b);
   
    init_color(id,r,g,b);

    init_pair(id,id,-1);


}
void set_color_bg(short id,const char *fg,const char *bg){
    short r1,g1,b1,r2,g2,b2;
    hex_to_rgb(fg,&r1,&g1,&b1);
    hex_to_rgb(bg,&r2,&g2,&b2);
    init_color(id,r1,g1,b1);
    init_color(id+60,r2,g2,b2);
    init_pair(id,id,id+60);


}



int load_theme(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
   
        return 1;
    }
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char key[64], value[64];

        char status[64];
        char selection[64];
        if (sscanf(line, "%63s = %63s", key, value) == 2) {

            if(strcmp(key,"KEYWORD") == 0){
                set_color(CP_KEYWORD, value);
            }
            else if(strcmp(key,"NORMAL") == 0){
                set_color(CP_DEFAULT, value);
            }
            else if(strcmp(key,"FUNCTION") == 0){
                set_color(CP_FUNCTION, value);
            }
            else if(strcmp(key,"COMMENT") == 0){
                set_color(CP_COMMENT, value);
            }
            else if(strcmp(key,"STRING") == 0){
                set_color(CP_STRING, value);
            }
            else if(strcmp(key,"CHAR") == 0){
                set_color(CP_CHAR, value);
            }
            else if(strcmp(key,"NUMBER") == 0){
                set_color(CP_NUMBER, value);
            }
            else if(strcmp(key,"BRACKET") == 0){
                set_color(CP_PAREN, value);
            }
            else if(strcmp(key,"SELECTION_BG") == 0){
                set_color_bg(CP_SELECTION, selection,value);
            }
            else if(strcmp(key,"STATUS_BG") == 0){
                set_color_bg(CP_STATUS, status,value);
            }
            else if(strcmp(key,"SELECTION") == 0){
                strcpy(selection,value);
            }
            else if(strcmp(key,"STATUS") == 0){
                strcpy(status,value);
            }
        }
    }

    fclose(f);
    return 0;
}

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
    #ifndef _WIN32
    const char* s = getenv("HOME");
    if(load_theme(strcat(s,"/.config/notec/theme.conf"))==0){
        return;
    }
    #endif
}
