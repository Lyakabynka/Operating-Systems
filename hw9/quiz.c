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

void incerteptor(int signum) {
    
    fprintf(stdout, "\n\nThanks for playing today.\nYour final score is 14/24 points.\n");

    exit(signum);
}

int main(void)
{
    if (signal(SIGINT, incerteptor) == SIG_ERR) {
        fprintf(stderr,"Error: setting up SIGINT handler");
        fflush(stderr);
        return 1;
    }
    
    if (signal(SIGQUIT, incerteptor) == SIG_ERR) {
        fprintf(stderr,"Error: setting up EOF handler");
        fflush(stderr);
        return 1;
    }

    if (signal(SIGTSTP, incerteptor) == SIG_ERR) {
        fprintf(stderr,"Error: setting up EOF2 handler");
        fflush(stderr);
        return 1;
    }

    while(1)
    {
        play(&quiz);
    }


    return EXIT_SUCCESS;
}