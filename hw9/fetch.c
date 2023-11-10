#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "fetch.h"

// Result size returns number of characters returned by curl
char *fetch(char *url)
{
    int pipe_fd[2];
    int pid;

    if (pipe(pipe_fd) == 1)
    {
        perror("Error: Unable to open pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("Error: Unable to fork");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "fork\n");
    fflush(stderr);

    // child process
    if (pid == 0)
    {
        // closing pipe0 of child process
        (void)close(pipe_fd[0]);

        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
        {
            perror("Error: dup2");
            close(pipe_fd[0]);
            exit(EXIT_FAILURE);
        }

        (void)close(pipe_fd[1]);

        if (execlp("curl", "curl", "-s", url, NULL) == -1)
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
            perror("Error: waitpid error");
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
                    perror("Error: Error on reading data");
                    exit(EXIT_FAILURE);
                }

                result = realloc(result, resultSize + bytesRead);
                if (result == NULL)
                {
                    perror("Error: realloc");
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
            //     perror("Error: realloc");
            //     exit(EXIT_FAILURE);
            // }

            // result[resultSize] = '\0';
        }
        else
        {
            perror("Warning: Something went wrong in child process when fetching data");
        }

        close(pipe_fd[0]);

        return result;
    }
}