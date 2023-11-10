#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/signal.h>

#include "fetch.h"
#include "parse.h"
#include "play.h"
#include "quiz.h"

char* url = "https://opentdb.com/api.php?amount=1&category=18&type=multiple";

quiz_t quiz = {
    .n = 0,
    .max = 0,
    .score = 0
};

void sigintHandler(int signum) {
    printf("Caught Ctrl+C signal (%d)\n", signum);
    // Add your cleanup or handling code here
    exit(signum);
}

// Signal handler for Ctrl+D (EOF)
void eofHandler(int signum) {
    printf("Caught Ctrl+D signal (%d)\n", signum);
    // Add your cleanup or handling code here
    exit(signum);
}

int main(void)
{
    if (signal(SIGINT, sigintHandler) == SIG_ERR) {
        perror("Error: setting up SIGINT handler");
        return 1;
    }
    
    if (signal(SIGQUIT, eofHandler) == SIG_ERR) {
        perror("Error: setting up EOF handler");
        return 1;
    }

    while(1)
    {
        play(&quiz);
    }


    return EXIT_SUCCESS;
}