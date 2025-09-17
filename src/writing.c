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






