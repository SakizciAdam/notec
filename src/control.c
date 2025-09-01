#include "control.h"

char* statusText;
int statusLength=0;

void initC(){
    statusText=malloc(sizeof(char)*200);
}

void setStatusText(const char* input){
    strncpy(statusText, input, 199);
    statusText[199] = '\0'; // Ensure null-termination
    statusLength = strlen(statusText);
}
#ifdef _WIN32
void renderC() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO sbInfo;
    GetConsoleScreenBufferInfo(hOut, &sbInfo);
    int columns = sbInfo.dwSize.X;
    int rows = sbInfo.dwSize.Y;
    int textRows = rows - 1; 

    int followIndex = (selEnd != -1) ? selEnd : getTextIndex();
    int fx = 0, fy = 0;
    for (int i = 0; i < followIndex && i < length; i++) {
        if (text[i] == '\n') { fy++; fx = 0; }
        else fx++;
    }

    if (fy < scrollY) scrollY = fy;
    if (fy >= scrollY + textRows) scrollY = fy - textRows + 1;
    if (fx < scrollX) scrollX = fx;
    if (fx >= scrollX + columns) scrollX = fx - columns + 1;

    char *screenBuf = malloc(columns * rows);
    WORD *attrBuf = malloc(columns * rows * sizeof(WORD));
    if (!screenBuf || !attrBuf) return;

    for (int i = 0; i < columns * rows; i++) {
        screenBuf[i] = ' ';
        attrBuf[i] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }

    int charIndex = 0;
    int currentLine = 0;
    while (currentLine < scrollY && charIndex < length) {
        if (text[charIndex] == '\n') currentLine++;
        charIndex++;
    }

    for (int y = 0; y < textRows; y++) {
        if (charIndex >= length) break;
        int lineStartIndex = charIndex;
        int lineLength = 0;
        while (lineStartIndex + lineLength < length && text[lineStartIndex + lineLength] != '\n') lineLength++;

        int renderStartIndex = lineStartIndex + scrollX;
        int renderEndIndex = lineStartIndex + lineLength;
        int charsToRender = renderEndIndex - renderStartIndex;
        if (charsToRender > columns) charsToRender = columns;

        for (int i = 0; i < charsToRender; i++) {
            int idx = renderStartIndex + i;
            int pos = y * columns + i;

            if (selStart != -1 && selEnd != -1) {
                int start = selStart < selEnd ? selStart : selEnd;
                int end   = selStart > selEnd ? selStart : selEnd;
                if (idx >= start && idx < end) attrBuf[pos] = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                else attrBuf[pos] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            } else {
                attrBuf[pos] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            }

            screenBuf[pos] = text[idx];
        }

        charIndex = lineStartIndex + lineLength;
        if (charIndex < length && text[charIndex] == '\n') charIndex++;
    }

    for (int x = 0; x < columns; x++) {
        int p = (rows - 1) * columns + x;
        screenBuf[p] = ' ';
        attrBuf[p] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }

    const char* prefix = "notec | v1.0 | ";
    int prefixLen = strlen(prefix);
    for (int i = 0; i < prefixLen && i < columns; i++) {
        int p = (rows - 1) * columns + i;
        screenBuf[p] = prefix[i];
    }
    for (int i = 0; i < statusLength && i + prefixLen < columns; i++) {
        int p = (rows - 1) * columns + prefixLen + i;
        screenBuf[p] = statusText[i];
    }

    DWORD written;
    WriteConsoleOutputCharacter(hOut, screenBuf, columns * rows, (COORD){0,0}, &written);
    WriteConsoleOutputAttribute(hOut, attrBuf, columns * rows, (COORD){0,0}, &written);

    int cx = 0, cy = 0;
    int finalIndex = (selEnd != -1) ? selEnd : getTextIndex();
    for (int i = 0; i < finalIndex && i < length; i++) {
        if (text[i] == '\n') { cy++; cx = 0; } else cx++;
    }
    COORD pos = { cx - scrollX, cy - scrollY };
    SetConsoleCursorPosition(hOut, pos);

    free(screenBuf);
    free(attrBuf);
}

void getInput(const char* prompt, char* buffer, int bufferSize) {
    int len = 0;
    buffer[0] = '\0';
    setStatusText(prompt);

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hIn, &mode);
    SetConsoleMode(hIn, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    bool done = false;
    while (!done) {
        char display[256];
        snprintf(display, sizeof(display), "%s%s", prompt, buffer);
        setStatusText(display);

        renderC();

        INPUT_RECORD rec;
        DWORD read;
        ReadConsoleInput(hIn, &rec, 1, &read);

        if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
            char c = rec.Event.KeyEvent.uChar.AsciiChar;

            if (c == '\r') {
                done = true;
            } else if (c == '\b') {
                if (len > 0) {
                    len--;
                    buffer[len] = '\0';
                }
            } else if (len < bufferSize - 1 && isprint(c)) {
                buffer[len++] = c;
                buffer[len] = '\0';
            }
        }
    }

    SetConsoleMode(hIn, mode);
}
#else
void getInput(const char* prompt, char* buffer, int bufferSize) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);     
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int len = 0;
    buffer[0] = '\0';
    bool done = false;

    while (!done) {
  
        char display[256];
        snprintf(display, sizeof(display), "%s%s", prompt, buffer);
        setStatusText(display);
        renderC();

        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == '\n' || c == '\r') {
                done = true;
            } else if (c == 127 || c == '\b') { // backspace on Linux is usually 127
                if (len > 0) {
                    len--;
                    buffer[len] = '\0';
                }
            } else if (isprint((unsigned char)c) && len < bufferSize - 1) {
                buffer[len++] = c;
                buffer[len] = '\0';
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore settings
}

void renderC() {
    int rows, columns;
    getmaxyx(stdscr, rows, columns);
    int textRows = rows - 1;

    erase();

    int charIndex = 0;
    int currentLine = 0;
    while (currentLine < scrollY && charIndex < length) {
        if (text[charIndex] == '\n') currentLine++;
        charIndex++;
    }

    for (int y = 0; y < textRows; y++) {
        if (charIndex >= length) break;

        int lineStartIndex = charIndex;
        int lineLength = 0;
        while (lineStartIndex + lineLength < length && text[lineStartIndex + lineLength] != '\n') 
            lineLength++;

        int renderStartIndex = lineStartIndex + scrollX;
        int renderEndIndex = lineStartIndex + lineLength;
        int charsToRender = renderEndIndex - renderStartIndex;
        if (charsToRender > columns) charsToRender = columns;

        for (int i = 0; i < charsToRender; i++) {
            int idx = renderStartIndex + i;

            if (selStart != -1 && selEnd != -1) {
                int start = selStart < selEnd ? selStart : selEnd;
                int end   = selStart > selEnd ? selStart : selEnd;
                if (idx >= start && idx < end) attron(A_REVERSE);
            }

            mvaddch(y, i, text[idx]);

            if (selStart != -1 && selEnd != -1) attroff(A_REVERSE);
        }

        charIndex = lineStartIndex + lineLength;
        if (charIndex < length && text[charIndex] == '\n') charIndex++;
    }

    move(rows - 1, 0);
    clrtoeol();
    printw("notec | v1.0 | %s", statusText);

    int cx = 0, cy = 0;
    int finalIndex = (selEnd != -1) ? selEnd : getTextIndex();
    for (int i = 0; i < finalIndex && i < length; i++) {
        if (text[i] == '\n') { cy++; cx = 0; }
        else cx++;
    }
    move(cy - scrollY, cx - scrollX);

    refresh();
}
#endif
void reset(){
    setStatusText(" ");
    renderC();

}

void moveSelectionUpLine() {
    if (selStart == -1) return; 
    int x = 0, y = 0;
    for (int i = 0; i < selEnd && i < length; i++) {
        if (text[i] == '\n') {
            y++;
            x = 0;
        } else {
            x++;
        }
    }

    if (y == 0) return; 
    y--; 

    int newIndex = 0;
    int cx = 0, cy = 0;
    while (newIndex < length && cy < y) {
        if (text[newIndex] == '\n') cy++;
        newIndex++;
    }

    while (newIndex < length && text[newIndex] != '\n' && cx < x) {
        cx++;
        newIndex++;
    }

    selEnd = newIndex; 
}

void moveSelectionDownLine() {
    if (selStart == -1) return;  
    int x = 0, y = 0;
    for (int i = 0; i < selEnd && i < length; i++) {
        if (text[i] == '\n') {
            y++;
            x = 0;
        } else {
            x++;
        }
    }

    if (y >= getMaxLine() - 1) return; 
    y++; 

    int newIndex = 0;
    int cx = 0, cy = 0;
    while (newIndex < length && cy < y) {
        if (text[newIndex] == '\n') cy++;
        newIndex++;
    }
    while (newIndex < length && text[newIndex] != '\n' && cx < x) {
        cx++;
        newIndex++;
    }
    selEnd = newIndex; 
}



bool selectionMode = false;

void toggleSelectionMode() {
    selectionMode = !selectionMode;
    if (!selectionMode) selStart = selEnd = -1;
    setStatusText(selectionMode ? "Selection mode" : "");
}

void moveCursorOrSelectionUp() {
    if (selectionMode) {
        if (selStart == -1) selStart = selEnd = getTextIndex();
        moveSelectionUpLine();
        setStatusText("Selected");
    } else moveCursorDown();
}

void moveCursorOrSelectionDown() {
    if (selectionMode) {
        if (selStart == -1) selStart = selEnd = getTextIndex();
        moveSelectionDownLine();
        setStatusText("Selected");
    } else moveCursorUp();
}

void moveCursorOrSelectionLeft() {
    if (selectionMode) {
        if (selStart == -1 && selEnd == -1) selStart = getTextIndex() - 1, selEnd = selStart + 2;
        selEnd--;
        if (selEnd == selStart) selStart = selEnd = -1, setStatusText("Unselected");
        else setStatusText("Selected");
    } else moveCursorLeft();
}

void moveCursorOrSelectionRight() {
    if (selectionMode) {
        if (selStart == -1 && selEnd == -1) selStart = selEnd = getTextIndex();
        selEnd++;
        if (selEnd >= length - 1) selEnd = length - 1;
        setStatusText("Selected");
    } else moveCursorRight();
}

void clearSelection() {
    selStart = selEnd = -1;
    setStatusText("Unselected");
}

void selectAll() {
    selStart = 0;
    selEnd = length;
}
#ifdef _WIN32
void copySelection() {
    if (selStart == -1 || selEnd == -1 || selStart == selEnd) return;
    setStatusText("Error");
    int start = selStart < selEnd ? selStart : selEnd;
    int end = selStart > selEnd ? selStart : selEnd;
    int lengthToCopy = end - start;
    if (lengthToCopy <= 0) return;
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, lengthToCopy + 1);
    if (!hMem) { CloseClipboard(); return; }
    memcpy(GlobalLock(hMem), text + start, lengthToCopy);
    GlobalUnlock(hMem);
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    setStatusText("Copied");
}

void pasteClipboard() {
    if (!OpenClipboard(NULL)) { setStatusText("Error"); return; }
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData) { setStatusText("No text in clipboard"); return; }
    wchar_t *wText = (wchar_t *)GlobalLock(hData);
    if (!wText) { setStatusText("Error"); CloseClipboard(); return; }
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wText, -1, NULL, 0, NULL, NULL);
    char *buffer = malloc(sizeNeeded);
    if (buffer) {
        setStatusText("Pasted");
        int oldIndex = getTextIndex();
        WideCharToMultiByte(CP_UTF8, 0, wText, -1, buffer, sizeNeeded, NULL, NULL);
        for (int i = 0; i < strlen(buffer); i++) {
            char ch = buffer[i];
            if ((int)ch == 13) {}
            else if ((int)ch == 9) handleKeyW('#');
            else if (isascii(ch)) handleKeyW(ch);
        }
        goToIndex(oldIndex);
    }
    GlobalUnlock(hData);
    CloseClipboard();
}
#else
void copySelection() {
    setStatusText("Copy not included in Linux version");
}

void pasteClipboard() {
    setStatusText("Paste not included in Linux version");
}
#endif
void goToLine() {
    reset();
    char input[20];
    getInput("Go to: ", input, sizeof(input));
    int lineNumber = atoi(input);
    if (lineNumber > getMaxLine() || lineNumber <= 0) {
        setStatusText("Line does not exist");
        return;
    }
    cursorY = lineNumber - 1;
    cursorX = 0;
}


void saveFile() {
    reset();
    if (fileSet) {
        char resp[4] = "";
        getInput("Save to current file? y/N: ", resp, sizeof(resp));
        if (!(resp[0] == 'y' || resp[0] == 'Y')) fileSet = false;
    }

    if (!fileSet) {
        char saveLocation[200];
        getInput("Save to: ", saveLocation, sizeof(saveLocation));
        //printf("\nSaving to %s\n", saveLocation);
        fileSet = true;
        if (fileName == NULL) fileName = malloc(strlen(saveLocation) + 1);
        else fileName = realloc(fileName, strlen(saveLocation) + 1);
        strcpy(fileName, saveLocation);
    }

    FILE *fptr = fopen(fileName, "r");
    if (fptr != NULL) {
        fclose(fptr);
        char resp[4] = "";
        getInput("File exists. Overwrite y/N: ", resp, sizeof(resp));
        if (!(resp[0] == 'y' || resp[0] == 'Y')) return;
    }

    fptr = fopen(fileName, "w");
    if (!fptr) {
        setStatusText("Error opening file");
        return;
    }

    if (length > 0) fprintf(fptr, "%s", text);
    fclose(fptr);
    setStatusText("File saved");
}



void findText() {
    char findText[256];
    getInput("Find: ", findText, sizeof(findText));
    int index = getTextIndex();
    char* subst = malloc(length - index + 1);
    strncpy(subst, text + index, length - index);
    subst[length - index] = '\0';
    char* result = strstr(subst, findText);
    if(result == NULL){
        setStatusText("Not found");
    } else {
        int position = result - subst + index;
        setStatusText("Found");
        goToIndex(position);
        cursorX += strlen(findText);
    }
    free(subst);
}

void handleKeyC(char c) {
    char lower = tolower(c);
    arrow = false;

    if (lower == 'q') exit(0);
    if (lower == 's') toggleSelectionMode();
    else if ((int)c == UP_ARROW) moveCursorOrSelectionUp();
    else if ((int)c == DOWN_ARROW) moveCursorOrSelectionDown();
    else if ((int)c == RIGHT_ARROW) moveCursorOrSelectionRight();
    else if ((int)c == LEFT_ARROW) moveCursorOrSelectionLeft();
    else if (lower == 'p') clearSelection();
    else if (lower == 'a') selectAll();
    else if (lower == 'c') copySelection();
    else if (lower == 'v') pasteClipboard();
    else if (lower == 'g') goToLine();
    else if (lower == 'w') saveFile();
    else if (lower == 'f') findText();
}
