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
