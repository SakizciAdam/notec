#include "init.h"


void init(){
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); init_colors();
    mode=WRITING_MODE;
    selStart=-1;
    selEnd=-1;
    

    text=malloc(sizeof(char));
    length=0;
    statusText=malloc(sizeof(char)*200);
    fileName = NULL;
    fileSet = false;
    set_status_text(":)");
}

void arg_parse(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (arg[0] == '-') {
     
            if (arg[1] == '-') {
                // --theme=./theme.conf
                if (strncmp(arg, "--theme=", 8) == 0) {
                    const char *themePath = arg + 8;

                    #ifdef _WIN32
                    printf("Error: Custom themes are not supported on Windows");
                    exit(1);
                    #endif
                    if (themePath && *themePath) {
                        if(load_theme(themePath)){
                            printf("Error: Failed to load theme %s\n",themePath);
                            exit(1);
                        }
                    } else {
                        printf("Error: --theme requires a path\n");
                        exit(1);
                    }
                }
                else {
                    printf("Unknown option: %s\n", arg);
                    exit(1);
                }
            }
          
            else {
                for (int j = 1; arg[j]; j++) {
                    switch (arg[j]) {
                        case 'v':
                            printf("notec v1.0 - By SakizciAdam\n"
                                   "If you encounter any issues or have a suggestion, "
                                   "please open an issue on GitHub.\n"
                                   "Contributions are always welcome!\n"
                                   "Thanks for using notec!\n");
                            exit(0);
                            break;
                        case 'r':
                            readOnly = true;
                            break;
                        default:
                            printf("Unknown option: -%c\n", arg[j]);
                            exit(1);
                    }
                }
            }
        }
        else if (!fileSet) {
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
                        if (new_text[k] == '\r' && k + 1 < len && new_text[k + 1] == '\n')
                            continue;
                        new_text[j++] = new_text[k];
                    }
                    new_text[j] = '\0';
                    length = j;
                    free(text);
                    text = new_text;
                }
                fclose(file);
            }
        }
        else {
            printf("Ignoring extra argument: %s\n", arg);
        }
    }
}
