#include "chlng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        return 1;
    }

    pclose(fortune_output);
    return -1;  // No text retrieved
}

int chlng_hide_word(chlng_t *c) {
    if (c == NULL || c->text == NULL) {
        return -1;  // Invalid challenge or text
    }

    char *word = strtok(c->text, " ");
    while (word != NULL) {
        if (strlen(word) > 3) {
            // Replace a word longer than 3 characters with underscores
            memset(word, '_', strlen(word));
            c->word = strdup(word);
            if (c->word == NULL) {
                perror("Memory allocation failed");
                return -1;
            }
            return 1;
        }
        word = strtok(NULL, " ");
    }
    return -1;  // No suitable word found to hide
}
