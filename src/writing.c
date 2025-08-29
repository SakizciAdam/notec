#include "writing.h"





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
    text=malloc(sizeof(char));
    length=0;
  
}

void handleKeyW(char c){
    if((int)c==-32){
        return;
    }
    if((int)c==8){
        //BACKSPACE
        int idx = getTextIndex();
        if (idx > 0 && length > 0) {
            for (int i = idx - 1; i < length - 1; i++) {
                text[i] = text[i + 1];
            }
            length--;
            text = realloc(text, length + 1);
            if (text) {
                text[length] = '\0';
            }
            if (cursorX > 0) {
                cursorX--;
            } else if (cursorY > 0) {
                cursorY--;
                cursorX = getLineLength(cursorY);
            }
        }
        return;
    }
    
    else if((int)c==72){
        if(cursorY>0){
            int previousLineLength=getLineLength(cursorY-1);

            if(cursorX<=previousLineLength){
                cursorY--;
            } else {
                cursorX=previousLineLength;
                cursorY--;
            }
        }
        
        return;
    }
    else if((int)c==80){
        if(cursorY+1==getMaxLine()){
            cursorX=getLineLength(cursorY);
            return;
        }
        cursorY++;
        if(cursorX>getLineLength(cursorY)){
            cursorX=getLineLength(cursorY);
        }
        return;
    }
    else if((int)c==75){
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
      

        return;
    }
    else if((int)c==77){
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

void renderW(){
    cls();

    CONSOLE_SCREEN_BUFFER_INFO sbInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &sbInfo);
    int columns = sbInfo.dwSize.X;
    int rows = sbInfo.dwSize.Y;
    int textRows = rows - 1; 

    if (cursorY < scrollY) {
        scrollY = cursorY;
    }
    if (cursorY >= scrollY + textRows) {
        scrollY = cursorY - textRows + 1;
    }
    if (cursorX < scrollX) {
        scrollX = cursorX;
    }
    if (cursorX >= scrollX + columns) {
        scrollX = cursorX - columns + 1;
    }

    int currentLine = 0;
    int charIndex = 0;

    while (currentLine < scrollY && charIndex < length) {
        if (text[charIndex] == '\n') {
            currentLine++;
        }
        charIndex++;
    }

    for (int y = 0; y < textRows; y++) {
        if (charIndex >= length) break;

        COORD linePos = {0, y};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), linePos);

        int lineStartIndex = charIndex;
        int lineLength = 0;
        while (lineStartIndex + lineLength < length && text[lineStartIndex + lineLength] != '\n') {
            lineLength++;
        }

        int renderStartIndex = lineStartIndex + scrollX;
        if (renderStartIndex < lineStartIndex + lineLength) {
            int charsToRender = lineLength - scrollX;
            if (charsToRender > columns) {
                charsToRender = columns;
            }
            if (charsToRender > 0) {
                fwrite(text + renderStartIndex, 1, charsToRender, stdout);
            }
        }
        
        charIndex = lineStartIndex + lineLength;
        if (charIndex < length && text[charIndex] == '\n') {
            charIndex++;
        }
    }


    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD statusBarPos = {0, rows - 1};
    SetConsoleCursorPosition(output, statusBarPos);
    char statusBar[100];
    float size = (float)length/1024;


    
    
    snprintf(statusBar, sizeof(statusBar), "Line: %d, Col: %d | Total Lines: %d | Size: %.1f kB", cursorY + 1, cursorX + 1, getMaxLine(),size);
    printf("%s", statusBar);
    

    COORD pos = {cursorX - scrollX, cursorY - scrollY};
    SetConsoleCursorPosition(output, pos);
}