#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int i=1;
    int on_newline = 1;

    
    if(argc < 2) return 0;
    
    
    if(strcmp(argv[0], "-n") == 0) {
        on_newline = 0; 
    }

    
    for(; i<argc; i++){
        fputs(argv[i], stdout);
        if(i < argc - 1){
            putchar(' ');
        }
    }
    if(on_newline) printf("\n");
        return 0;
}

