#include "control.h"





void set_status_text(const char* input){
    strncpy(statusText, input, 199);
    statusText[199] = '\0'; // Ensure null-termination
    statusLength = strlen(statusText);
}
#ifdef _WIN32

void get_input(const char* prompt, char* buffer, int bufferSize) {
    int len = 0;
    buffer[0] = '\0';
    set_status_text(prompt);

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hIn, &mode);
    SetConsoleMode(hIn, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    bool done = false;
    while (!done) {
        char display[256];
        snprintf(display, sizeof(display), "%s%s", prompt, buffer);
        set_status_text(display);

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
void get_input(const char* prompt, char* buffer, int bufferSize) {
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
        set_status_text(display);
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
    set_status_text(" ");
    render();

}

void move_selection_up_line() {
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

void move_selection_down_line() {
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

    if (y >= get_max_line() - 1) return; 
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

void toggle_selection_mode() {
    selectionMode = !selectionMode;
    if (!selectionMode) selStart = selEnd = -1;
    set_status_text(selectionMode ? "Selection mode" : "");
}

void move_cursor_or_selection_up() {
 
    if (selectionMode) {
        if (selStart == -1) selStart = selEnd = get_text_index();
        move_selection_up_line();
        set_status_text("Selected");
    } else move_cursor_up();
}

void move_cursor_or_selection_down() {
    if (selectionMode) {
        if (selStart == -1) selStart = selEnd = get_text_index();
        move_selection_down_line();
        set_status_text("Selected");
    } else move_cursor_down();
}

void move_cursor_or_selection_left() {
    if (selectionMode) {
        if (selStart == -1 && selEnd == -1) selStart = get_text_index() - 1, selEnd = selStart + 2;
        selEnd--;
        if (selEnd == selStart) selStart = selEnd = -1, set_status_text("Unselected");
        else set_status_text("Selected");
    } else move_cursor_left();
}

void move_cursor_or_selection_right() {
    if (selectionMode) {
        if (selStart == -1 && selEnd == -1) selStart = selEnd = get_text_index();
        selEnd++;
        if (selEnd >= length - 1) selEnd = length - 1;
        set_status_text("Selected");
    } else move_cursor_right();
}

void clear_selection() {
    selStart = selEnd = -1;
    set_status_text("Unselected");
}

void select_all() {
    selStart = 0;
    selEnd = length;
}

void goto_line() {
    reset();
    char input[20];
    get_input("Go to: ", input, sizeof(input));
    int lineNumber = atoi(input);
    if (lineNumber > get_max_line() || lineNumber <= 0) {
        set_status_text("Line does not exist");
        return;
    }
    cursorY = lineNumber - 1;
    cursorX = 0;
}


void save_file() {
    reset();
    if (fileSet) {
        char resp[4] = "";
        get_input("Save to current file? y/N: ", resp, sizeof(resp));
        if (!(resp[0] == 'y' || resp[0] == 'Y')) fileSet = false;
    }

    if (!fileSet) {
        char saveLocation[200];
        get_input("Save to: ", saveLocation, sizeof(saveLocation));
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
        get_input("File exists. Overwrite y/N: ", resp, sizeof(resp));
        if (!(resp[0] == 'y' || resp[0] == 'Y')) return;
    }

    fptr = fopen(fileName, "w");
    if (!fptr) {
        set_status_text("Error opening file");
        return;
    }

    if (length > 0) fprintf(fptr, "%s", text);
    fclose(fptr);
    set_status_text("File saved");
    saved=1;
}

void quit(){
    char resp[256];

    if(length>0&&saved==0){
        get_input("Unsaved file. Quit without saving? y/N ", resp, sizeof(resp));

        if(strcmp(resp,"y")==0||strcmp(resp,"Y")==0){
            printf("Quitting");
            exit(0);
        } else {
            set_status_text(":)");
        }
    } else {
        exit(0);
    }
    

   
}

void find_text() {
    char findText[256];
    get_input("Find: ", findText, sizeof(findText));
    int index = get_text_index();
    char* subst = malloc(length - index + 1);
    strncpy(subst, text + index, length - index);
    subst[length - index] = '\0';
    char* result = strstr(subst, findText);
    if(result == NULL){
        set_status_text("Not found");
    } else {
        int position = result - subst + index;
        set_status_text("Found");
        goto_index(position);
        cursorX += strlen(findText);
    }
    free(subst);
}
