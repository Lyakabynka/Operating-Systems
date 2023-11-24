#include "chlng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

chlng_t *chlng_new(void) {
    chlng_t *new_chlng = (chlng_t *)malloc(sizeof(chlng_t));
    if (new_chlng == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_chlng->text = NULL;
    new_chlng->word = NULL;
    return new_chlng;
}

void chlng_reset(chlng_t *c) {
    if (c) {
        free(c->text);
        free(c->word);
        c->text = NULL;
        c->word = NULL;
    }
}

void chlng_del(chlng_t *c) {
    if (c) {
        chlng_reset(c);
        free(c);
    }
}

int chlng_fetch_text(chlng_t *c) {
    // Simulating fetching text from 'fortune' command
    // Replace this with your actual logic to obtain phrases
    FILE *fortune_output = popen("fortune", "r");
    if (fortune_output == NULL) {
        perror("Error running fortune command");
        return -1;
    }

    char buffer[512];
    if (fgets(buffer, sizeof(buffer), fortune_output) != NULL) {
        c->text = strdup(buffer);
        if (c->text == NULL) {
            perror("Memory allocation failed");
            pclose(fortune_output);
            return -1;
        }
        fprintf(stderr, c->text);
        return 1;
    }

    pclose(fortune_output);
    return -1;  // No text retrieved
}

int chlng_hide_word(chlng_t* c) {
    char* working_text = strdup(c->text);

    srand(time(NULL));

    char* word_to_hide = NULL;
    char* token = strtok(working_text, " ");
    int word_count = 0;

    while (token != NULL) {
        word_count++;

        if (rand() % word_count == 0) {
            word_to_hide = token;
        }

        token = strtok(NULL, " ");
    }

    if (word_to_hide != NULL) {
        size_t word_len = strlen(word_to_hide);
        c->word = malloc((word_len + 1) * sizeof(char));
        if (c->word == NULL) {
            free(working_text);
            return 0;
        }
        strcpy(c->word, word_to_hide);

        char* found = strstr(c->text, word_to_hide);
        while (found != NULL) {
            memset(found, '_', strlen(word_to_hide));
            found = strstr(found + 1, word_to_hide);
        }

        free(working_text);
        return 1;
    }

    free(working_text);
    return 0;
}
