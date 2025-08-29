#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include "control.h"

#define WRITING_MODE 0
#define CONTROL_MODE 1


int main(int argc, char **argv) {
    int mode=WRITING_MODE;
    initW();
    initC();
    if(argc>=2){

     
        FILE *file = fopen(argv[1], "rb"); 
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

                free(text); 
                text = new_text; 
                length = len;

            }
            fclose(file);
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

    return 0;
}