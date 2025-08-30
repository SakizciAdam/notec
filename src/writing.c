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

void handleKeyW(char c){
    if((int)c==-9999){
        return;
    }
    if((int)c==-32){
        arrow=true;
        return;
    }
    if ((int)c == 8&&!readOnly) {
         // BACKSPACE
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

            selStart = -1;
            selEnd = -1;
        } else {

            int idx = getTextIndex();
            if (idx > 0 && length > 0) {
                for (int i = idx - 1; i < length - 1; i++) {
                    text[i] = text[i + 1];
                }
                length--;
                text = realloc(text, length + 1);
                if (text) text[length] = '\0';

                if (cursorX > 0) {
                    cursorX--;
                } else if (cursorY > 0) {
                    cursorY--;
                    cursorX = getLineLength(cursorY);
                }
            }
        }
        return;
    }
    
    else if((int)c==72&&arrow){
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
        
        return;
    }
    else if((int)c==80&&arrow){
        if(cursorY+1==getMaxLine()){
            cursorX=getLineLength(cursorY);
            return;
        }
        cursorY++;
        if(cursorX>getLineLength(cursorY)){
            cursorX=getLineLength(cursorY);
        }
        arrow=false;
        return;
    }
    else if((int)c==75&&arrow){
        //LEFT
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
        return;
    }
    else if((int)c==77&&arrow){
        //RIGHT
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
        return;
    }

    if(readOnly){
        return;
    }

    if((int)c==9){
        //TAB
        for(int i=0;i<4;i++){
            addCharAt(' ',getTextIndex());
            cursorX++;
        }

        return;
    }
    if((int)c==13){
        addCharAt('\n',getTextIndex());
        cursorY++;
        cursorX=0;
        return;
    }
    if(isascii(c)){
        addCharAt(c,getTextIndex());
        cursorX++;
    }
    
}

void renderW() {
    cls(); 

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO sbInfo;
    GetConsoleScreenBufferInfo(hOut, &sbInfo);
    int columns = sbInfo.dwSize.X;
    int rows = sbInfo.dwSize.Y;
    int textRows = rows - 1; 

    if (cursorY < scrollY) scrollY = cursorY;
    if (cursorY >= scrollY + textRows) scrollY = cursorY - textRows + 1;
    if (cursorX < scrollX) scrollX = cursorX;
    if (cursorX >= scrollX + columns) scrollX = cursorX - columns + 1;

    int currentLine = 0;
    int charIndex = 0;
    while (currentLine < scrollY && charIndex < length) {
        if (text[charIndex] == '\n') currentLine++;
        charIndex++;
    }

    for (int y = 0; y < textRows; y++) {
        if (charIndex >= length) break;

        COORD linePos = {0, y};
        SetConsoleCursorPosition(hOut, linePos);

        int lineStartIndex = charIndex;
        int lineLength = 0;
        while (lineStartIndex + lineLength < length && text[lineStartIndex + lineLength] != '\n') {
            lineLength++;
        }

        int renderStartIndex = lineStartIndex + scrollX;
        int renderEndIndex = lineStartIndex + lineLength;
        if (renderStartIndex < renderEndIndex) {
            int charsToRender = renderEndIndex - renderStartIndex;
            if (charsToRender > columns) charsToRender = columns;

            int i = 0;
            while (i < charsToRender) {
                int idx = renderStartIndex + i;
                char c = text[idx];

                if (selStart != -1 && selEnd != -1 && idx >= selStart && idx < selEnd) {
                    SetConsoleTextAttribute(hOut, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    putchar(c);
                    i++;
                    continue;
                }

                if (c == '/' && idx + 1 < length && text[idx + 1] == '/') {
                    SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_BLUE); // cyan-ish
                    while (i < charsToRender) {
                        putchar(text[renderStartIndex + i]);
                        i++;
                    }
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    continue;
                }
                if (c == '"') {
                    SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
                    putchar(c);
                    i++;
                    while (i < charsToRender) {
                        idx = renderStartIndex + i;
                        putchar(text[idx]);
                        if (text[idx] == '"') {
                            i++;
                            break;
                        }
                        i++;
                    }
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    continue;
                }

                if (isdigit(c)) {
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN); 
                    putchar(c);
                    i++;
                    while (i < charsToRender && isdigit(text[renderStartIndex + i])) {
                        putchar(text[renderStartIndex + i]);
                        i++;
                    }

                    if (i < charsToRender) {
                        char suffix = text[renderStartIndex + i];
                        if (suffix == 'f' || suffix == 'F' || suffix == 'd' || suffix == 'D' ||
                            suffix == 'u' || suffix == 'U' || suffix == 'l' || suffix == 'L') {
                            putchar(suffix);
                            i++;
                        }
                    }

                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 
                    continue;
                }
                if (c == '#' && i == 0) { 
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE);
                    while (i < charsToRender) {
                        char pc = text[renderStartIndex + i];
                        putchar(pc);
                        i++;
                        if (pc == '\n') break;
                    }
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    continue;
                }
                if (c == '\'') {
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN); 
                    putchar(c);
                    i++;
                    while (i < charsToRender) {
                        idx = renderStartIndex + i;
                        char pc = text[idx];
                        putchar(pc);
                        i++;

                        if (pc == '\'' && text[idx - 1] != '\\') {
                            break;
                        }
                    }
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 
                    continue;
                }
                if ((c == '=' && idx + 1 < length && text[idx + 1] == '=') ||
                    c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']') {

                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY); 
                    putchar(c);

                    if (c == '=' && text[idx + 1] == '=') {
                        putchar('=');
                        i++; 
                    }

                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 
                    i++;
                    continue;
                }
                if (isalpha(c) || c == '_') {
                    char buf[64];
                    int bi = 0;
                    int j = i;
                    while (j < charsToRender && (isalnum(text[renderStartIndex + j]) || text[renderStartIndex + j] == '_') && bi < 63) {
                        buf[bi++] = text[renderStartIndex + j];
                        j++;
                    }
                    buf[bi] = '\0';

                    const char *keywords[] = {"int","char","if","else","for","while","return","void","struct","typedef","float","double"
                    ,"uint","int32","short","ushort","long","let","var","const"}; //probably missing a ton
                    int isKw = 0;
                    for (int k = 0; k < sizeof(keywords)/sizeof(keywords[0]); k++) {
                        if (strcmp(buf, keywords[k]) == 0) { isKw = 1; break; }
                    }

                    if (isKw) {
                        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN); 
                    } else {
                        int nextIdx = renderStartIndex + j;
                        while (nextIdx < length && (text[nextIdx] == ' ' || text[nextIdx] == '\t')) nextIdx++;
                        if (nextIdx < length && text[nextIdx] == '(') {
                            SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY); 
                        }
                    }

                    for (int k = 0; k < bi; k++) putchar(buf[k]);
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 

                    i = j;
                    continue;
                }

                SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                putchar(c);
                i++;
            }
        }

        charIndex = lineStartIndex + lineLength;
        if (charIndex < length && text[charIndex] == '\n') charIndex++;
    }
    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    COORD statusBarPos = {0, rows - 1};
    SetConsoleCursorPosition(hOut, statusBarPos);
    char statusBar[200];
    float size = (float)length / 1024.0f;
    snprintf(statusBar, sizeof(statusBar), "Line: %d, Col: %d | Total Lines: %d | Size: %.1f kB", cursorY + 1, cursorX + 1, getMaxLine(), size);

    if (readOnly) {
        strcat(statusBar, " | Readonly");
    }

    if (fileSet) {
        char *t = (char*)malloc(50 * sizeof(char));
        sprintf(t, " | %s", fileName);
        strcat(statusBar, t);
        free(t);
    }

    printf("%-*s", columns, statusBar); 
    COORD pos = {cursorX - scrollX, cursorY - scrollY};
    SetConsoleCursorPosition(hOut, pos);

    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}