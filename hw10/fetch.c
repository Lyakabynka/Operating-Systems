#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "fetch.h"

char *fetch()
{
    int pipe_fd[2];
    int pid;

    if (pipe(pipe_fd) == 1)
    {
        fprintf(stderr,"Error: Unable to open pipe");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1)
    {
        fprintf(stderr,"Error: Unable to fork");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    // child process
    if (pid == 0)
    {
        // closing pipe0 of child process
        (void)close(pipe_fd[0]);

        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            fprintf(stderr,"Error: dup2");
            fflush(stderr);
            close(pipe_fd[0]);
            exit(EXIT_FAILURE);
        }

        (void)close(pipe_fd[1]);

        if (execlp("fortune", "fortune", "-s", NULL, NULL) == -1)
        {
            fprintf(stderr, "Error: execlp1");
            fflush(stderr);
        }

        fprintf(stderr, "Error: execlp2");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    else
    {
        char *result = NULL;

        close(pipe_fd[1]);

        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            fprintf(stderr,"Error: waitpid error");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
        {
            size_t resultSize = 0;
            char buffer[512];
            ssize_t bytesRead, bytesWritten;

            // reading from pipe_fd[0] ( output of child process )
            while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
            {
                if (bytesRead == -1)
                {
                    fprintf(stderr,"Error: Error on reading data");
                    fflush(stderr);
                    exit(EXIT_FAILURE);
                }

                result = realloc(result, resultSize + bytesRead);
                if (result == NULL)
                {
                    fprintf(stderr,"Error: realloc");
                    fflush(stderr);
                    exit(EXIT_FAILURE);
                }

                memcpy(result + resultSize, buffer, bytesRead);
                resultSize += bytesRead;
            }

            result = realloc(result, resultSize + 1);
            result[resultSize] = '\0';

            resultSize++;

            // result = realloc(result, resultSize + 1);
            // if(result == NULL)
            // {
            //     fprintf(stderr,"Error: realloc");
            //     exit(EXIT_FAILURE);
            // }

            // result[resultSize] = '\0';
        }
        else
        {
            fprintf(stderr,"Warning: Something went wrong in child process when fetching data");
            fflush(stderr);
        }

        close(pipe_fd[0]);

        return result;
    }
}