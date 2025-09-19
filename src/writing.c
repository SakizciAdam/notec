#include "writing.h"

int saved=0;

int get_text_index(){
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

void goto_index(int index) {
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

int get_line_length(int lineIndex){
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



void add_substring_at(const char* substr, int index) {
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


void add_char_at(char c,int index){
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




void move_cursor_down(){
    if(cursorY+1==get_max_line()){
        cursorX=get_line_length(cursorY);
        return;
    }
    cursorY++;
    if(cursorX>get_line_length(cursorY)){
        cursorX=get_line_length(cursorY);
    }
    arrow=false;
}

void move_cursor_up(){
    if(cursorY>0){
        int previousLineLength=get_line_length(cursorY-1);

        if(cursorX<=previousLineLength){
            cursorY--;
        } else {
            cursorX=previousLineLength;
            cursorY--;
        }
    }
    arrow=false;
}

void move_cursor_right(){
    if(cursorX==get_line_length(cursorY)){
        if(cursorY+1>=get_max_line()){
            return;
        }
        cursorY++;
            
        cursorX=0;
        return;
    }
    cursorX++;
    arrow=false;
}

void move_cursor_left(){
    cursorX--;
    if(cursorX<0){
        if(cursorY>0){
            cursorY--;
            cursorX=get_line_length(cursorY);
        } else {
            cursorX=0;
        }
            
    }
      
    arrow=false;
}






