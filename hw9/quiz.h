#ifndef QUIZ_H
#define QUIZ_H

typedef struct
{
    unsigned n;
    unsigned score;
    unsigned max;
    char *question;
    char *answer;
    char *choices[4];
} quiz_t;

#endif