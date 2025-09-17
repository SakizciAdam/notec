
#include "init.h"
#include "input.h"


int main(int argc, char **argv) {

    init();
    arg_parse(argc,argv);

    render();
    while(true){
        if(kbhit()){

            char c;

            #ifdef _WIN32
            c=_getch();
            #else
            c=getch();
            #endif
            
            /*
            printf("%d %c\n",c,c);

            if(true){
                continue;
            }*/
            on_key(c,mode);
            render();
          
        }
    }

    endwin();

    return 0;
}