#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

char* fetch(char*url)
{
    int pipe_fd[2];
    pid_t pid;

    if(pipe(pipe_fd)==1)
    {
        perror("Error: Unable to open pipe");
        exit(EXIT_FAILURE);
    }

    
}