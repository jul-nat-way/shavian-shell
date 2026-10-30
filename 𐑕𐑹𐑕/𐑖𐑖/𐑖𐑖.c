#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char *shsh_read_line(void);
char **shsh_split_line(char *line);
void shsh_loop();


int main(int argc, char **argv)
{
    // load any config files here
    
    // run command loop
    shsh_loop();

    // end program
    return EXIT_SUCCESS;
}

// builtin shell functions (each one needs a latin and shavian version)


//function declarations
int bi_cd(char **args);
int bi_help(char **args);
int bi_exit(char **args);

// list of builtin commands
char *builtin_str[] = {
    "cd", "𐑗𐑛",
    "help", "𐑣𐑧𐑤𐑐",
    "exit", "𐑧𐑜𐑟𐑦𐑑"
};

// and their cooresponding functions
int (*builtin_func[]) (char **) = {
    &bi_cd,
    &bi_help,
    &bi_exit
};

int shsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// implementation of the builtins
int bi_cd(char **args){
    if(args[1] == NULL) {
        fprintf(stderr, "𐑖𐑖: 𐑧𐑒𐑕𐑐𐑧𐑒𐑑𐑦𐑛 𐑼𐑜 𐑑 \"𐑗𐑛\"\n");
    } else {
        if (chdir(args[1]) != 0){
            perror("𐑖𐑖");
        }
    }
    return 1;
}

int bi_help(char **args) {
    printf(" A shell written entirely for use in Shavian \n ");
    printf(" Created by Julian Way \n");
    printf(" --------------------- \n");
    printf(" 𐑢𐑧𐑤𐑒𐑩𐑥 𐑑 𐑖𐑱𐑝𐑾𐑯 𐑖𐑧𐑤 (𐑖𐑖) \n");
    printf(" 𐑒𐑮𐑦𐑧𐑑𐑦𐑛 𐑚𐑲 ⸰𐑡𐑵𐑤𐑾𐑯 ⸰𐑢𐑱 \n\n");
    printf(" type \"exit\" or \"𐑧𐑜𐑟𐑦𐑑\" to exit \n");
    return 1;
}

int bi_exit(char **args) {
    return 0;
}


// Parsing functions
#define SHSH_BUFFSIZE 1024
char *shsh_read_line(void)
{
    int buffsize = SHSH_BUFFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffsize);
    int c; 
    
    if(!buffer)
    {
        fprintf(stderr, "𐑖𐑖: 𐑨𐑤𐑴𐑒𐑱𐑖𐑩𐑯 𐑧𐑮𐑼 -- allocation error");
        exit(EXIT_FAILURE);
    }

    for(;;) 
    {
        //  Read a character
        c = getchar();

        // if we hit EOF, replace it with a null character and go to the next line (#smart)
        if (c == EOF || c == '\n') 
        {
            buffer[position] = '\0';
            return buffer;
        } else 
        {
            buffer[position] = c; 
        }
        position++; 

        // reallocate if we exceed the buffer size
        if(position >= buffsize)
        {
            buffsize += SHSH_BUFFSIZE; 
            buffer = realloc(buffer, buffsize * sizeof(char));
            if(!buffer)
            {
                fprintf(stderr, "𐑖𐑖: 𐑨𐑤𐑴𐑒𐑱𐑖𐑩𐑯 𐑧𐑮𐑼 -- allocation error");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define SHSH_TOK_BUFFSIZE 64
#define SHSH_TOK_DELIM " \t\r\n\a"
char **shsh_split_line(char *line)
{
    int buffsize = SHSH_TOK_BUFFSIZE;
    int position = 0; 
    char **tokens = malloc(sizeof(char*) * buffsize);
    char *token;

    if(!tokens){
        fprintf(stderr, "𐑖𐑖: 𐑨𐑤𐑴𐑒𐑱𐑖𐑩𐑯 𐑧𐑮𐑼 -- allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHSH_TOK_DELIM);
    while(token != NULL){
        tokens[position] = token;
        position++;

        if(position >= buffsize) {
            buffsize += SHSH_TOK_BUFFSIZE;
            tokens = realloc(tokens, buffsize * sizeof(char));
            if(!tokens){
                fprintf(stderr, "𐑖𐑖: 𐑨𐑤𐑴𐑒𐑱𐑖𐑩𐑯 𐑧𐑮𐑼 -- allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SHSH_TOK_DELIM);
    }
    tokens[position] = NULL;

    return tokens;
}

int shsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if(execvp(args[0], args) == -1) {
            perror("𐑖𐑖");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("𐑖𐑖");
    }
    else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        }
        while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

//the acutal execution function, gets called every loop
int shsh_execute(char **args)
{
    int i;

    //check if command was empty
    if(args[0] == NULL){
        return 1;
    }

    //check if command was builtin
    for(int i=0; i<shsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            //gets divided by 2 because every bulitin has 2 versions
            return (*builtin_func[(int)i/2])(args); 
        }
    }
    
    //run any other command
    return shsh_launch(args);
}

void shsh_loop()
{
    // Read -> Parse -> Execute
    char *line;
    char **args;
    int status;

    do 
    { 
        //the command prompt
        printf("𐑖𐑬 𐑖𐑧𐑤 >> ");
        line = shsh_read_line();
        args = shsh_split_line(line);
        
        status = shsh_execute(args);

        free(line);
        free(args);
    } 
    while (status == 1);
}
