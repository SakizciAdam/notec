#include "control.h"

#define WRITING_MODE 0
#define CONTROL_MODE 1

int main(int argc, char **argv) {
    #ifndef _WIN32 
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); initColors();
    #endif
    
    int mode=WRITING_MODE;
    initW();
    initC();
    fileName = NULL;
    fileSet = false;

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (arg[0] == '-') {
            for (int j = 1; arg[j]; j++) {
                switch (arg[j]) {
                    case 'v':
                        printf("notec v1.0 - By SakizciAdam\nIf you encounter any issues or have a suggestion, please open a issue in GitHub.\nContributions are always welcome!\nThanks for using notec!");
                        return 0;
                    case 'r':
                        readOnly = true;
                        break;
                    default:
                        printf("Unknown option: -%c\n", arg[j]);
                        return 1;
                }
            }
        } else if (!fileSet) {
            FILE *file = fopen(arg, "rb");
            if (!file) {
                fileName = malloc(strlen(arg) + 1);
                strcpy(fileName, arg);
                fileSet = true;
            } else {
                
                fileName = malloc(strlen(arg) + 1);
                strcpy(fileName, arg);
                fileSet = true;

                fseek(file, 0, SEEK_END);
                long len = ftell(file);
                fseek(file, 0, SEEK_SET);

                if (len >= 3) {
                    unsigned char bom[3];
                    fread(bom, 1, 3, file);
                    if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)) {
                        fseek(file, 0, SEEK_SET);
                    } else {
                        len -= 3;
                    }
                }

                char *new_text = malloc(len + 1);
                if (new_text) {
                    fread(new_text, 1, len, file);
                    new_text[len] = '\0';
                    int j = 0;
                    for (int k = 0; k < len; k++) {
                        if (new_text[k] == '\r' && k + 1 < len && new_text[k + 1] == '\n') continue;
                        new_text[j++] = new_text[k];
                    }
                    new_text[j] = '\0';
                    length = j;
                    free(text);
                    text = new_text;
                }

            }

            fclose(file);
        } else {
            printf("Ignoring extra argument: %s\n", arg);
        }
    }
    


    renderW();
    while(true){
        if(kbhit()){

            char c=getch();
            if((int)c==27){
                if(mode>1){
                    mode=0;
                }
                
                mode=1-mode;
                
                c=-9999;
            }
            /*
            printf("%d %c\n",c,c);

            if(true){
                continue;
            }*/
            
            if(mode==WRITING_MODE){
                handleKeyW(c);

                renderW();
            } 
            if(mode==CONTROL_MODE){
                handleKeyC(c);
                renderC();

            }
          
        }
    }
    #ifndef _WIN32 
    endwin();
    #endif
    return 0;
}