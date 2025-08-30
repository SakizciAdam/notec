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

            for (int i = 0; i < charsToRender; i++) {
                int idx = renderStartIndex + i;

           
                if (selStart != -1 && selEnd != -1 && idx >= selStart && idx < selEnd) {
                    SetConsoleTextAttribute(hOut, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                } else {
                    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }

                putchar(text[idx]);
            }
        }

        charIndex = lineStartIndex + lineLength;
        if (charIndex < length && text[charIndex] == '\n') charIndex++;
    }
    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    COORD statusBarPos = {0, rows - 1};
    SetConsoleCursorPosition(hOut, statusBarPos);

    for (int i = 0; i < columns; i++) printf(" ");
    SetConsoleCursorPosition(hOut, statusBarPos);

    const char* prefix = "notec | v1.0 | ";
    printf("%s", prefix);

    if (statusLength > 0) {
        printf("%s", statusText);
    }


    COORD pos = {columns, rows - 1};
    SetConsoleCursorPosition(hOut, pos);


    SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void reset(){
    setStatusText(" ");
    renderC();

}
char* backupText;

void moveSelectionUpLine() {
    if (selStart == -1 || selEnd == -1) return;


    int x = 0, y = 0;
    for (int i = 0; i < selStart && i < length; i++) {
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

    selStart = newIndex;
}

void moveSelectionDownLine() {
    if (selStart == -1 || selEnd == -1) return;


    int x = 0, y = 0;
    for (int i = 0; i < selStart && i < length; i++) {
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

    selStart = newIndex;
}

void handleKeyC(char c) {
    if(c=='q'){
        free(text);
        free(statusText);
        exit(0);
    }
    if((int)c==72){
        moveSelectionUpLine();
        return;
    }
    if((int)c==80){
        moveSelectionDownLine();
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
    if(c=='a'){
        selStart=0;
        selEnd=length;
    }
    if(c=='c'){
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
    if(c=='v'){
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

    if(c=='g'){
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
    if(c=='w'){
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
    if(c=='f'){
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
