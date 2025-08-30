#include "utils.h"

bool readOnly=false;
char* fileName;
bool fileSet=false;


void cls() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;

    // Fill the entire buffer with spaces
    FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X * csbi.dwSize.Y, (COORD){0,0}, &written);

    // Reset all attributes
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, (COORD){0,0}, &written);

    // Move cursor back to top-left
    SetConsoleCursorPosition(hConsole, (COORD){0,0});
}
