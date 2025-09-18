#include "draw.h"
#include <stdbool.h>


int get_max_line(){
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

void render() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    int textRows = rows - 1;

    if (textRows < 1) return;

    if (cursorY < scrollY) scrollY = cursorY;
    if (cursorY >= scrollY + textRows) scrollY = cursorY - textRows + 1;
    if (cursorX < scrollX) scrollX = cursorX;
    if (cursorX >= scrollX + cols) scrollX = cursorX - cols + 1;

    erase();

    static const char *keywords[] = {
        "int","char","if","else","for","while","return","void","struct","typedef",
        "float","double","uint","int32","short","ushort","long","let","var","const",
        "switch","case","break","continue","default","sizeof","do","enum","static",
        "extern","inline","signed","unsigned","goto","null","NULL","ifdef","endif",
        "true","True","false","False","boolean","bool",
    };
    const int nKeywords = sizeof(keywords) / sizeof(keywords[0]);

    bool hasSelection = (selStart != -1 && selEnd != -1);
    int selA = 0, selB = 0;
    if (hasSelection) {
        selA = selStart < selEnd ? selStart : selEnd;
        selB = selStart > selEnd ? selStart : selEnd;
    }

    int charIndex = 0;
    int currentLine = 0;
    while (currentLine < scrollY && charIndex < length) {
        if (text[charIndex] == '\n') currentLine++;
        charIndex++;
    }

    for (int y = 0; y < textRows && charIndex < length; y++) {
        if (charIndex >= length) break;

        int lineStart = charIndex;
        int lineLen = 0;
        while (lineStart + lineLen < length && text[lineStart + lineLen] != '\n') lineLen++;

        int renderStart = lineStart + scrollX;
        int renderEnd = lineStart + lineLen;
        int charsToRender = (renderStart < renderEnd) ? (renderEnd - renderStart) : 0;
        if (charsToRender > cols) charsToRender = cols;

        int i = 0;
        while (i < charsToRender) {
            int absIdx = renderStart + i;
            char c = text[absIdx];

            if (c == '/' && absIdx + 1 < lineStart + lineLen && text[absIdx + 1] == '/') {
                int tokenColor = CP_COMMENT;
                for (int k = i; k < charsToRender; k++) {
                    int a = renderStart + k;
                    int selected = hasSelection && (a >= selA && a < selB);
                    int pair = selected ? CP_SELECTION : tokenColor;
                    attron(COLOR_PAIR(pair));
                    mvaddch(y, k, text[a]);
                    attroff(COLOR_PAIR(pair));
                }
                i = charsToRender;
                break;
            }

            if (c == '"') {
                int tokenColor = CP_STRING;
                int k = 0;
                int closed = 0;
                while (i + k < charsToRender) {
                    int a = renderStart + i + k;
                    char ch = text[a];
                    int selected = hasSelection && (a >= selA && a < selB);
                    int pair = selected ? CP_SELECTION : tokenColor;

                    attron(COLOR_PAIR(pair));
                    mvaddch(y, i + k, ch);
                    attroff(COLOR_PAIR(pair));

                    if (ch == '"' && k > 0) {
          
                        int back = a - 1, bs = 0;
                        while (back >= lineStart && text[back] == '\\') { bs++; back--; }
                        if ((bs % 2) == 0) { closed = 1; k++; break; }
                    }
                    k++;
                }

       
                if (!closed) k = charsToRender - i;

                i += k;
                continue;
            }


            if (c == '\'') {
                int tokenColor = CP_CHAR;
                int k = 0;
                int closed = 0;
                while (i + k < charsToRender) {
                    int a = renderStart + i + k;
                    char ch = text[a];
                    int selected = hasSelection && (a >= selA && a < selB);
                    int pair = selected ? CP_SELECTION : tokenColor;

                    attron(COLOR_PAIR(pair));
                    mvaddch(y, i + k, ch);
                    attroff(COLOR_PAIR(pair));

                    if (ch == '\'' && k > 0) {
        
                        int back = a - 1, bs = 0;
                        while (back >= lineStart && text[back] == '\\') { bs++; back--; }
                        if ((bs % 2) == 0) { closed = 1; k++; break; }
                    }
                    k++;
                }


                if (!closed) k = charsToRender - i;

                i += k;
                continue;
            }

            if (isdigit((unsigned char)c)) {
                int tokenColor = CP_NUMBER;
                int k = 0;
                while (i + k < charsToRender) {
                    char ch = text[renderStart + i + k];
                    if (!isdigit((unsigned char)ch) && ch != '.' && ch != '_') break;
                    int a = renderStart + i + k;
                    int selected = hasSelection && (a >= selA && a < selB);
                    int pair = selected ? CP_SELECTION : tokenColor;
                    attron(COLOR_PAIR(pair));
                    mvaddch(y, i + k, ch);
                    attroff(COLOR_PAIR(pair));
                    k++;
                }
                if (i + k < charsToRender) {
                    char ch = text[renderStart + i + k];
                    if (ch=='f'||ch=='F'||ch=='d'||ch=='D'||ch=='u'||ch=='U'||ch=='l'||ch=='L') {
                        int a = renderStart + i + k;
                        int selected = hasSelection && (a >= selA && a < selB);
                        int pair = selected ? CP_SELECTION : tokenColor;
                        attron(COLOR_PAIR(pair));
                        mvaddch(y, i + k, ch);
                        attroff(COLOR_PAIR(pair));
                        k++;
                    }
                }
                if (k==0) { attron(COLOR_PAIR(tokenColor)); mvaddch(y, i, c); attroff(COLOR_PAIR(tokenColor)); i++; }
                else { i += k; }
                continue;
            }

            if (c == '#' && absIdx + 7 < lineStart + lineLen && strncmp(&text[absIdx], "#include", 8) == 0) {
                int tokenColor = CP_INCLUDE;
                int kwlen = 8;
                for (int k = 0; k < kwlen && i + k < charsToRender; k++) {
                    int a = renderStart + i + k;
                    int selected = hasSelection && (a >= selA && a < selB);
                    int pair = selected ? CP_SELECTION : tokenColor;
                    attron(COLOR_PAIR(pair));
                    mvaddch(y, i + k, text[a]);
                    attroff(COLOR_PAIR(pair));
                }
                i += kwlen;
                continue;
            } else if(c=='#'){
                int tokenColor = CP_PAREN;
                int selected = hasSelection && (absIdx >= selA && absIdx < selB);
                int pair = selected ? CP_SELECTION : tokenColor;
                attron(COLOR_PAIR(pair));
                mvaddch(y, i, c);
                attroff(COLOR_PAIR(pair));
                i++;
                continue;
            }

            if (isalpha((unsigned char)c) || c == '_') {
                int j = 0;
                while (i + j < charsToRender) {
                    char ch = text[renderStart + i + j];
                    if (!isalnum((unsigned char)ch) && ch != '_') break;
                    j++;
                }
                int idStartAbs = renderStart + i;
                int idLen = j;
                int isKw = 0;
                for (int kk = 0; kk < nKeywords; kk++) {
                    if ((int)strlen(keywords[kk]) == idLen && strncmp(&text[idStartAbs], keywords[kk], idLen) == 0) { isKw = 1; break; }
                }
                int isFunc = 0;
                if (!isKw) {
                    int nextAbs = idStartAbs + idLen;
                    while (nextAbs < lineStart + lineLen && isspace((unsigned char)text[nextAbs])) nextAbs++;
                    if (nextAbs < lineStart + lineLen && text[nextAbs] == '(') isFunc = 1;
                }
                int tokenColor = CP_DEFAULT;
                if (isKw) tokenColor = CP_KEYWORD;
                else if (isFunc) tokenColor = CP_FUNCTION;
                else tokenColor = CP_DEFAULT;
                for (int k = 0; k < idLen && i + k < charsToRender; k++) {
                    int a = idStartAbs + k;
                    int selected = hasSelection && (a >= selA && a < selB);
                    int pair = selected ? CP_SELECTION : tokenColor;
                    attron(COLOR_PAIR(pair));
                    mvaddch(y, i + k, text[a]);
                    attroff(COLOR_PAIR(pair));
                }
                i += idLen;
                continue;
            }

            if ( (c == '+') ||(c == '-') ||(c == '(') || (c == ')') || (c == '{') || (c == '}') || (c == '[') || (c == ']') || (c == '=') || (c == '<')|| (c == '>')|| (c == '!')) {
                int tokenColor = CP_PAREN;
                int selected = hasSelection && (absIdx >= selA && absIdx < selB);
                int pair = selected ? CP_SELECTION : tokenColor;
                attron(COLOR_PAIR(pair));
                mvaddch(y, i, c);
                attroff(COLOR_PAIR(pair));
                i++;
                continue;
            }

            if (
                (c == '&' && absIdx + 1 < lineStart + lineLen && text[absIdx + 1] == '&') ||
                (c == '|' && absIdx + 1 < lineStart + lineLen && text[absIdx + 1] == '|') ) {
                int tokenColor = CP_PAREN;
                int sel0 = hasSelection && (absIdx >= selA && absIdx < selB);
                int sel1 = hasSelection && (absIdx+1 >= selA && absIdx+1 < selB);
                int pair0 = sel0 ? CP_SELECTION : tokenColor;
                int pair1 = sel1 ? CP_SELECTION : tokenColor;
                attron(COLOR_PAIR(pair0)); mvaddch(y, i, text[absIdx]); attroff(COLOR_PAIR(pair0));
                if (i+1 < charsToRender) { attron(COLOR_PAIR(pair1)); mvaddch(y, i+1, text[absIdx+1]); attroff(COLOR_PAIR(pair1)); }
                i += 2;
                continue;
            }

            { 
                int selected = hasSelection && (absIdx >= selA && absIdx < selB);
                int pair = selected ? CP_SELECTION : CP_DEFAULT;
                attron(COLOR_PAIR(pair));
                mvaddch(y, i, c);
                attroff(COLOR_PAIR(pair));
                i++;
            }
        }

        charIndex = lineStart + lineLen;
        if (charIndex < length && text[charIndex] == '\n') charIndex++;
    }

    move(rows - 1, 0);
    attron(COLOR_PAIR(CP_STATUS));
    char statusBar[512];
    if (mode == CONTROL_MODE) {
        if (statusLength > 0 && statusText) {
            snprintf(statusBar, sizeof(statusBar), "CONTROL | notec | v1.0 | %s", statusText);
        } else {
            snprintf(statusBar, sizeof(statusBar), "CONTROL | notec | v1.0");
        }
    } else {
        float sizeKB = (float)length / 1024.0f;
        if (fileSet && fileName) {
            snprintf(statusBar, sizeof(statusBar), "WRITING | Line: %d, Col: %d | Total Lines: %d | Size: %.1f kB | %s%s",
                     cursorY+1, cursorX+1, get_max_line(), sizeKB,
                     readOnly ? "Readonly | " : "",
                     fileName);
        } else {
            snprintf(statusBar, sizeof(statusBar), "WRITING | Line: %d, Col: %d | Total Lines: %d | Size: %.1f kB%s",
                     cursorY+1, cursorX+1, get_max_line(), sizeKB,
                     readOnly ? " | Readonly" : "");
        }
    }
    mvaddnstr(rows - 1, 0, statusBar, cols);
    clrtoeol();
    attroff(COLOR_PAIR(CP_STATUS));

    int cx = cursorX - scrollX;
    int cy = cursorY - scrollY;
    if (cx < 0) cx = 0;
    if (cy < 0) cy = 0;
    if (cy >= textRows) cy = textRows - 1;
    if (cx >= cols) cx = cols - 1;
    move(cy, cx);

    refresh();
}