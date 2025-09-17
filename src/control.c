#include "control.h"





void setStatusText(const char* input){
    strncpy(statusText, input, 199);
    statusText[199] = '\0'; // Ensure null-termination
    statusLength = strlen(statusText);
}
#ifdef _WIN32

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

        render();

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
        render();

        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == '\n' || c == '\r') {
                done = true;
            } else if (c == 127 || c == '\b') { 
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


#endif


void reset(){
    setStatusText(" ");
    render();

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
    } else moveCursorUp();
}

void moveCursorOrSelectionDown() {
    if (selectionMode) {
        if (selStart == -1) selStart = selEnd = getTextIndex();
        moveSelectionDownLine();
        setStatusText("Selected");
    } else moveCursorDown();
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
