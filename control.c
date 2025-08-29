#include "control.h"

char* statusText;
int statusLength=0;

void initC(){
    statusText=malloc(sizeof(char)*200);
}

void setStatusText(const char* input){
    int len=sizeof(input)/sizeof(char);

    statusText=input;
    statusLength=len;
}

void renderC() {
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

    // Render
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

    for (int i = 0; i < columns; i++) {
        printf(" ");
    }
    SetConsoleCursorPosition(output, statusBarPos);
  
    const char* prefix = "notec | v1.0 | ";
    printf("%s", prefix);

    if (statusLength > 0) {
        printf("%s", statusText);
    }
    COORD pos = { columns, rows - 1 };
    SetConsoleCursorPosition(output, pos);
    
}

void reset(){
    setStatusText(" ");
    renderC();

}

void handleKeyC(char c) {
    if(c=='q'){
        free(text);
        free(statusText);
        exit(0);
    }
    if(c=='b'){
        reset();
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
        printf("Save to: ");
        char saveLocation[200];
        scanf("%s",&saveLocation);
        printf("\nSaving to %s\n",saveLocation);
        FILE *fptr;

        // Open a file in append mode
        fptr = fopen(saveLocation, "r");
        if(fptr!=NULL){
            cls();
            printf("File exists. Overwrite y/N");
            char resp;
            scanf(" %c",&resp);

            if(resp!='y'&&resp!='Y'){
                return;
            }
            fclose(fptr);
        } 

        fptr = fopen(saveLocation, "w");
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
