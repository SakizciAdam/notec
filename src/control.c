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

void handleKeyC(char c) {
    char lower=tolower(c);
    arrow=false;
    if(lower=='q'){
        exit(0);
    }
    if((int)c==72){
        if(selStart == -1) selStart = selEnd = getTextIndex();
        moveSelectionUpLine();
        setStatusText("Selected");
        return;
    }
    if((int)c==80){
        if(selStart == -1) selStart = selEnd = getTextIndex();
        moveSelectionDownLine();
        setStatusText("Selected");
        return;
    }
    if((int)c==77){
        //RIGHT
        if(selStart==-1&&selEnd==-1){
            selStart=getTextIndex();
            selEnd=selStart;
        }
        selEnd++;
        if(selEnd>=length-1){
            selEnd=length-1;
        }
        setStatusText("Selected");
    }
    if((int)c==75){
        //LEFT
        if(selStart==-1&&selEnd==-1){
            selStart=getTextIndex()-1;
            selEnd=selStart+2;
        }
        selEnd--;
       
        setStatusText("Selected");
        if(selEnd==selStart){
            selEnd=selStart=-1;
            setStatusText("Unselected");
        }
    }
    
    if(lower=='a'){
        selStart=0;
        selEnd=length;
    }
    if(lower=='c'){
        if (selStart == -1 || selEnd == -1 || selStart == selEnd) {
            return; 
        }
        setStatusText("Error");
        int start = selStart < selEnd ? selStart : selEnd;
        int end = selStart > selEnd ? selStart : selEnd;
        int lengthToCopy = end - start;

        if (lengthToCopy <= 0) return;

  
        if (!OpenClipboard(NULL)) return;

  
        EmptyClipboard();


        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, lengthToCopy + 1);
        if (!hMem) {
            CloseClipboard();
            
            return;
        }


        memcpy(GlobalLock(hMem), text + start, lengthToCopy);
        GlobalUnlock(hMem);


        SetClipboardData(CF_TEXT, hMem);

        CloseClipboard();
        setStatusText("Copied");
    }
    if(lower=='v'){
        if (!OpenClipboard(NULL)) {
            setStatusText("Error");

            return;
        }

        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData == NULL) {
            setStatusText("No text in clipboard");

            return;
        }

        wchar_t *wText  = (wchar_t *)GlobalLock(hData);
        if (wText  == NULL) {
            setStatusText("Error");
            CloseClipboard();
            return;
        }
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wText, -1, NULL, 0, NULL, NULL);
        char *buffer = malloc(sizeNeeded);
        if (buffer) {
            setStatusText("Pasted");
            int oldIndex=getTextIndex();
            WideCharToMultiByte(CP_UTF8, 0, wText, -1, buffer, sizeNeeded, NULL, NULL);
            for(int i=0;i<strlen(buffer);i++){
                char ch=buffer[i];
         
                if((int)ch==13){
                    //handleKeyW('x');
                }
                else if((int)ch==9){
                    handleKeyW('#');
                }
                else if(isascii(ch)){
                    handleKeyW(ch);
                    
                }
            }
            goToIndex(oldIndex);
            //free(buffer);
        }

   
        GlobalUnlock(hData);
        CloseClipboard();
    }

    if(lower=='g'){
        reset();
        printf("Go to: ");
        int lineNumber;
        scanf("%d",&lineNumber);

        if(lineNumber>getMaxLine()||lineNumber<=0){
            setStatusText("Line does not exist");
            return;
        }
        cursorY=lineNumber-1;
        cursorX=0;
    }
    if(lower=='w'){
        reset();

        if(fileSet){
            printf("Save to %s? y/N",fileName);

            char resp;
            scanf(" %c",&resp);

            if(resp=='y'||resp=='Y'){
          

            } else {
                fileSet=false;
            }
        }

        if(!fileSet){
            reset();
            printf("Save to: ");
            char saveLocation[200];
            scanf("%s",&saveLocation);
            printf("\nSaving to %s\n",saveLocation);
            fileSet=true;
            if(fileName==NULL){
                fileName=malloc(sizeof(char)*strlen(saveLocation));
            } else {
                fileName=realloc(fileName,sizeof(char)*strlen(saveLocation));
            }
            strcpy(fileName,saveLocation);
        }
        FILE *fptr;

        fptr = fopen(fileName, "r");
        if(fptr!=NULL){
            reset();
            printf("File exists. Overwrite y/N? ");
            char resp;
            scanf(" %c",&resp);

            if(resp!='y'&&resp!='Y'){
                return;
            }
            fclose(fptr);
        } 

        fptr = fopen(fileName, "w");
        if(length>0){
            fprintf(fptr, text);
        }
        fclose(fptr);
        
        setStatusText("File saved");
    }

    if(lower=='f'){
        reset();
   
        int count=0;
        printf("Find: ");
        char find[200];
        scanf("%s",&find);
        int index=getTextIndex();
        char* subst=malloc(sizeof(char)*(length-index));

        
        strncpy(subst, text + index, length-index);

        subst[length-index] = '\0';

        char *result = strstr(subst, find);

        if(result==NULL){
            setStatusText("Not found");
            return;
        }

        int position = result - subst + index;
        count++;
        char status[50];
        sprintf(status,"Found %d",count);
        setStatusText(status);
        goToIndex(position);
        cursorX+=strlen(find);
    }
}
