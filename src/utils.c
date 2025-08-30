#include "utils.h"
#include <stdio.h>

bool readOnly = false;
char* fileName = NULL;
bool fileSet = false;

void cls() {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD written;

        if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;

        FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X * csbi.dwSize.Y, (COORD){0,0}, &written);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, (COORD){0,0}, &written);
        SetConsoleCursorPosition(hConsole, (COORD){0,0});
    #else
   
        printf("\033[2J\033[H");
        fflush(stdout);
    #endif
}
#ifndef _WIN32
bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}
#endif