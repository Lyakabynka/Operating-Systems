#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quiz.h"
#include "parse.h"
#include "fetch.h"

#define MAX_PTNS 8

int play(quiz_t *quiz)
{
    char *msg = fetch("https://opentdb.com/api.php?amount=1&category=18&type=multiple");

    if (parse(quiz, msg) == -1)
    {
        free(quiz->choices[0]);
        free(quiz->choices[1]);
        free(quiz->choices[2]);
        free(quiz->choices[3]);
        free(quiz->question);
        perror("Error: unable to parse json\n");
        return 0;
    }

    // free message after parsing
    free(msg);

    fprintf(stdout, "%s\n\n", quiz->question);

    fprintf(stdout, "[a] %s\n", quiz->choices[0]);
    fprintf(stdout, "[b] %s\n", quiz->choices[1]);
    fprintf(stdout, "[c] %s\n", quiz->choices[2]);
    fprintf(stdout, "[d] %s\n\n", quiz->choices[3]);

    int avail_points = MAX_PTNS;

    quiz->max += MAX_PTNS;

    char input;

    for (;;)
    {

        fprintf(stdout, "(%dpt) > ", avail_points);

        read(STDIN_FILENO, &input, 1);
        
        if(input < 97 || input > 100)
        {
            continue;
        }

        if (strcmp(quiz->choices[input - 97], quiz->answer) == 0)
        {
            quiz->score += avail_points;
            fprintf(stdout, "Congratulation, answer [%c] is correct. Your current score is %d/%d points.\n\n", input, quiz->score, quiz->max);

            break;
        }

        fprintf(stdout, "Answer [%c] is wrong, try again.\n", input);
        avail_points = avail_points / 2;

    }

    free(quiz->choices[0]);
    free(quiz->choices[1]);
    free(quiz->choices[2]);
    free(quiz->choices[3]);
    free(quiz->question);

    return 0;
}