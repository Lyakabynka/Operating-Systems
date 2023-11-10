#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <jansson.h>
#include <time.h>
#include <string.h>

#include "quiz.h"

void shuffle(char **array, size_t size)
{
    for (size_t i = size - 1; i > 0; --i)
    {
        size_t j = rand() % (i + 1);

        // swapping
        char *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int parse(quiz_t *quiz, char *msg)
{
    json_error_t error;

    json_t *root, *results, *result,
        *question,
        *answer,

        *incorrect_answers,
        *choices[4];

    root = json_loads(msg, 0, &error);
    if (!root)
    {
        fprintf(stderr, "Error: unable to parse JSON: %s", error.text);
        return -1;
    }

    results = json_object_get(root, "results");

    result = json_array_get(results, 0);

    //
    question = json_object_get(result, "question");
    answer = json_object_get(result, "correct_answer");
    //

    incorrect_answers = json_object_get(result, "incorrect_answers");

    //
    choices[0] = answer;
    choices[1] = json_array_get(incorrect_answers, 0);
    choices[2] = json_array_get(incorrect_answers, 1);
    choices[3] = json_array_get(incorrect_answers, 2);
    //

    quiz->question = strdup(json_string_value(question));
    quiz->answer = strdup(json_string_value(answer));
    quiz->choices[0] = strdup(json_string_value(choices[0]));
    quiz->choices[1] = strdup(json_string_value(choices[1]));
    quiz->choices[2] = strdup(json_string_value(choices[2]));
    quiz->choices[3] = strdup(json_string_value(choices[3]));

    json_decref(root);

    shuffle(quiz->choices, 4);

    return 0;
}