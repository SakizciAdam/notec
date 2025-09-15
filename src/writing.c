#include "writing.h"

char* text = NULL;
int length = 0;
int cursorX = 0;
int cursorY = 0;
int scrollX = 0;
int scrollY = 0;
int selStart = -1;
int selEnd = -1;
bool arrow=false;

int getTextIndex(){
    int x=0;
    int y=0;
    for(int i=0;i<length;i++){
        if(x==cursorX&&y==cursorY){
            return i;
        }

        if(text[i]=='\n'){
            y++;
            x=0;
        } else {
            x++;
        }
    }
    return length;
}

void goToIndex(int index) {
    int x = 0, y = 0;
    for (int i = 0; i < index && i < length; i++) {
        if (text[i] == '\n') {
            y++;
            x = 0;
        } else {
            x++;
        }
    }
    cursorX = x;
    cursorY = y;
}

int getLineLength(int lineIndex){
    int currentLine = 0;
    int lineLength = 0;
    int i = 0;

    while (currentLine < lineIndex && i < length) {
        if (text[i] == '\n') {
            currentLine++;
        }
        i++;
    }

    if (currentLine == lineIndex) {
        while (i < length && text[i] != '\n') {
            lineLength++;
            i++;
        }
    }
    
    return lineLength;
}

int getMaxLine(){
    int lines = 0;
    for(int i=0; i < length; i++){
        if(text[i] == '\n'){
            lines++;
        }
    }
    if(text[length-1]!='\n'){
        lines++;
    }
    return lines;
}


void addSubstringAt(const char* substr, int index) {
    if (!substr || index < 0 || index > length) {
        return;
    }
    int substr_len = strlen(substr);
    if (substr_len == 0) {
        return;
    }
    char *new_text = realloc(text, length + substr_len + 1);
    if (!new_text) {
        return;
    }
    text = new_text;
    for (int i = length - 1; i >= index; i--) {
        text[i + substr_len] = text[i];
    }
    memcpy(text + index, substr, substr_len);
    length += substr_len;
    text[length] = '\0';
}


void addCharAt(char c,int index){
    if (index < 0 || index > length) {
        return;
    }
    length++;
    char *new_text = realloc(text, length + 1);
    if (new_text == NULL) {
        length--; 
        return;
    }
    text = new_text;

    for (int i = length - 1; i > index; i--) {
        text[i] = text[i - 1];
    }
    text[index] = c;
    text[length] = '\0';
}


void initW(){
    selStart=-1;
    selEnd=-1;
    

    text=malloc(sizeof(char));
    length=0;
  
}

void moveCursorDown(){
    if(cursorY+1==getMaxLine()){
        cursorX=getLineLength(cursorY);
        return;
    }
    cursorY++;
    if(cursorX>getLineLength(cursorY)){
        cursorX=getLineLength(cursorY);
    }
    arrow=false;
}

void moveCursorUp(){
    if(cursorY>0){
        int previousLineLength=getLineLength(cursorY-1);

        if(cursorX<=previousLineLength){
            cursorY--;
        } else {
            cursorX=previousLineLength;
            cursorY--;
        }
    }
    arrow=false;
}

void moveCursorRight(){
    if(cursorX==getLineLength(cursorY)){
        if(cursorY+1>=getMaxLine()){
            return;
        }
        cursorY++;
            
        cursorX=0;
        return;
    }
    cursorX++;
    arrow=false;
}

void moveCursorLeft(){
    cursorX--;
    if(cursorX<0){
        if(cursorY>0){
            cursorY--;
            cursorX=getLineLength(cursorY);
        } else {
            cursorX=0;
        }
            
    }
      
    arrow=false;
}

void handleKeyW(char c) {
    if ((int)c == -9999) return;
    if ((int)c == -32) { arrow = true; return; }

    if (readOnly) return;

    int idx = getTextIndex();

    if ((int)c == BACKSPACE) {
        if (selStart != -1 && selEnd != -1 && selStart != selEnd) {
            int start = selStart < selEnd ? selStart : selEnd;
            int end = selStart > selEnd ? selStart : selEnd;
            int delLength = end - start;

            for (int i = start; i < length - delLength; i++) {
                text[i] = text[i + delLength];
            }
            length -= delLength;
            text = realloc(text, length + 1);
            if (text) text[length] = '\0';

            goToIndex(start);

            selStart = selEnd = -1;
        } else if (idx > 0 && length > 0) {
            for (int i = idx - 1; i < length - 1; i++) {
                text[i] = text[i + 1];
            }
            length--;
            text = realloc(text, length + 1);
            if (text) text[length] = '\0';

            if (cursorX > 0) cursorX--;
            else if (cursorY > 0) {
                cursorY--;
                cursorX = getLineLength(cursorY);
            }
        }
        return;
    }
    #ifndef _WIN32 
    arrow=true;
    #endif
    if (arrow) {
        switch ((int)c) {
            case DOWN_ARROW: moveCursorDown(); return;
            case UP_ARROW: moveCursorUp(); return;
            case LEFT_ARROW: moveCursorLeft(); return;
            case RIGHT_ARROW: moveCursorRight(); return;
        }
    }

    switch ((int)c) {
        case 9: 
            for (int i = 0; i < 4; i++) {
                addCharAt(' ', idx++);
                cursorX++;
            }
            break;
        case RETURN:
            addCharAt('\n', idx);
            cursorY++;
            cursorX = 0;
            break;
        default:
            if ((unsigned char)c <= 127) {
                addCharAt(c, idx);
                cursorX++;
            }
            break;
    }
}
const char *keywords[] = {"int","char","if","else","for","while","return","void","struct","typedef","float","double"
                    ,"uint","int32","short","ushort","long","let","var","const"}; //probably missing a ton


void renderW() {
    int rows, columns;
    getmaxyx(stdscr, rows, columns);
    int textRows = rows - 1;

    if (cursorY < scrollY) scrollY = cursorY;
    if (cursorY >= scrollY + textRows) scrollY = cursorY - textRows + 1;
    if (cursorX < scrollX) scrollX = cursorX;
    if (cursorX >= scrollX + columns) scrollX = cursorX - columns + 1;

    erase();

    int charIndex = 0;
    int currentLine = 0;
    while (currentLine < scrollY && charIndex < length) {
        if (text[charIndex] == '\n') currentLine++;
        charIndex++;
    }


    int numKeywords = sizeof(keywords)/sizeof(keywords[0]);

    for (int y = 0; y < textRows; y++) {
        if (charIndex >= length) break;

        int lineStart = charIndex;
        int lineLen = 0;
        while (lineStart + lineLen < length && text[lineStart + lineLen] != '\n') lineLen++;

        int renderStart = lineStart + scrollX;
        int renderEnd = lineStart + lineLen;
        int i = 0;
        while (i < lineLen && i < columns) {
            int idx = renderStart + i;
            if (idx >= length) break;
            char c = text[idx];

            int pair = CP_DEFAULT;
            bool customHandled = false;

            if (selStart != -1 && selEnd != -1 && idx >= selStart && idx < selEnd) {
                attron(COLOR_PAIR(CP_SELECTION));
                mvaddch(y, i, c);
                attroff(COLOR_PAIR(CP_SELECTION));
                i++; 
                continue;
            }

            if (c == '/' && idx + 1 < length && text[idx+1] == '/') {
                attron(COLOR_PAIR(CP_COMMENT));
                for (int j = i; j < lineLen && j < columns; j++) {
                    mvaddch(y, j, text[lineStart + scrollX + j]);
                }
                attroff(COLOR_PAIR(CP_COMMENT));
                break;
            }

            if (c == '"') {
                attron(COLOR_PAIR(CP_STRING));
                mvaddch(y, i, c);
                i++;
                while (i < lineLen && i < columns) {
                    int idx2 = renderStart + i;
                    mvaddch(y, i, text[idx2]);
                    if (text[idx2] == '"') { i++; break; }
                    i++;
                }
                attroff(COLOR_PAIR(CP_STRING));
                continue;
            }

            if (c == '\'') {
                attron(COLOR_PAIR(CP_CHAR));
                mvaddch(y, i, c);
                i++;
                while (i < lineLen && i < columns) {
                    int idx2 = renderStart + i;
                    mvaddch(y, i, text[idx2]);
                    if (text[idx2] == '\'') { i++; break; }
                    i++;
                }
                attroff(COLOR_PAIR(CP_CHAR));
                continue;
            }

            if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']') {
                attron(COLOR_PAIR(CP_BRACKET));
                mvaddch(y, i, c);
                attroff(COLOR_PAIR(CP_BRACKET));
                i++;
                continue;
            }

            if (isdigit((unsigned char)c)) {
                attron(COLOR_PAIR(CP_NUMBER));
                mvaddch(y, i, c);
                i++;
                while (i < lineLen && i < columns && isdigit((unsigned char)text[renderStart + i])) {
                    mvaddch(y, i, text[renderStart + i]);
                    i++;
                }
                if (i < lineLen && i < columns) {
                    char s = text[renderStart + i];
                    if (s=='f'||s=='F'||s=='d'||s=='D'||s=='u'||s=='U'||s=='l'||s=='L') {
                        mvaddch(y, i, s);
                        i++;
                    }
                }
                attroff(COLOR_PAIR(CP_NUMBER));
                continue;
            }

            if (c == '#' && idx + 7 < length) {
                if (strncmp(&text[idx], "#include", 8) == 0) {
                    attron(COLOR_PAIR(CP_PREPROC));
                    for (int j = 0; j < 8 && i+j < columns; j++) {
                        mvaddch(y, i + j, text[idx + j]);
                    }
                    attroff(COLOR_PAIR(CP_PREPROC));
                    i += 8;
                    continue;
                }
            }

            if (isalpha((unsigned char)c) || c == '_') {
                char buf[64];
                int bi = 0;
                int j = i;
                while (j < lineLen && j < columns && (isalnum((unsigned char)text[renderStart + j]) || text[renderStart + j] == '_') && bi < 63) {
                    buf[bi++] = text[renderStart + j];
                    j++;
                }
                buf[bi] = '\0';

                int isKw = 0;
                for (int k = 0; k < numKeywords; k++) {
                    if (strcmp(buf, keywords[k]) == 0) { isKw = 1; break; }
                }

                int isFunc = 0;
                if (!isKw) {
                    int nextIdx = renderStart + j;
                    while (nextIdx < length && isspace((unsigned char)text[nextIdx])) nextIdx++;
                    if (nextIdx < length && text[nextIdx] == '(') isFunc = 1;
                }

                if (isKw) {
                    attron(COLOR_PAIR(CP_KEYWORD));
                    for (int k = 0; k < bi; k++) mvaddch(y, i + k, buf[k]);
                    attroff(COLOR_PAIR(CP_KEYWORD));
                } else if (isFunc) {
                    attron(COLOR_PAIR(CP_FUNCTION));
                    for (int k = 0; k < bi; k++) mvaddch(y, i + k, buf[k]);
                    attroff(COLOR_PAIR(CP_FUNCTION));
                } else {
                    for (int k = 0; k < bi; k++) mvaddch(y, i + k, buf[k]);
                }

                i = j;
                continue;
            }

            mvaddch(y, i, c);
            i++;
        }

        charIndex = lineStart + lineLen;
        if (charIndex < length && text[charIndex] == '\n') charIndex++;
    }

    // status bar
    move(rows - 1, 0);
    clrtoeol();
    char statusBar[256];
    float sizeKB = (float)length / 1024.0f;
    snprintf(statusBar, sizeof(statusBar), "Line: %d, Col: %d | Total Lines: %d | Size: %.1f kB",
             cursorY+1, cursorX+1, getMaxLine(), sizeKB);
    if (readOnly) strcat(statusBar, " | Readonly");
    if (fileSet && fileName) {
        char t[128]; snprintf(t, sizeof(t), " | %s", fileName);
        strncat(statusBar, t, sizeof(statusBar) - strlen(statusBar) - 1);
    }
    attron(A_REVERSE);
    mvaddnstr(rows - 1, 0, statusBar, columns);
    attroff(A_REVERSE);

    move(cursorY - scrollY, cursorX - scrollX);
    refresh();
}


