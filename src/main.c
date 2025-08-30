#include "control.h"

#define WRITING_MODE 0
#define CONTROL_MODE 1

int main(int argc, char **argv) {
    int mode=WRITING_MODE;
    initW();
    initC();
    fileName=malloc(sizeof(char));
    if(argc>=2){

     
        FILE *file = fopen(argv[1], "rb"); 
        fileName=realloc(fileName,sizeof(char)*(strlen(argv[1])+1));
        fileSet=true;
        strcpy(fileName,argv[1]);
        if (file) {
            fseek(file, 0, SEEK_END);
            long len = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (len >= 3) {
                unsigned char bom_check[3];
                fread(bom_check, 1, 3, file);
                if (bom_check[0] == 0xEF && bom_check[1] == 0xBB && bom_check[2] == 0xBF) {
                    len -= 3; 
                } else {
   
                    fseek(file, 0, SEEK_SET);
                }
            }
            
            char *new_text = malloc(len + 1);
            if (new_text) {
                fread(new_text, 1, len, file);
                new_text[len] = '\0';
                int j = 0;
                for (int i = 0; i < len; i++) {
                    if (new_text[i] == '\r' && i + 1 < len && new_text[i + 1] == '\n') {
                        continue; 
                    }
                    new_text[j++] = new_text[i];
                }
                new_text[j] = '\0';
                len = j;

                free(text);
                text = new_text;
                length = len;
            }
            fclose(file);
        }

        int len=argc;
        for(int i=2;i<len;i++){
            char* arg=argv[i];
            int argLen=strlen(arg)-1;
            char *option=malloc(sizeof(char)*(argLen+1));

            for(int j=0;j<strlen(arg);j++){
                char b=arg[j];

                if(j==0&&b!='-'){
                    printf("Unknown argument %s\n",arg);
                    return 1;
                }
                option[j]=b;
                
            }

            if(strcmp(option,"-r")){
                readOnly=true;
            }
            
            free(option);
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
                
                c=-32;
            }
            //printf("%d %c\n",c,c);
            
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
    free(fileName);
    return 0;
}