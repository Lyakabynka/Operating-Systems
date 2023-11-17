#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "chlng.h"

chlng_t *chlng_new() {
    chlng_t *new_challenge = (chlng_t *)malloc(sizeof(chlng_t));
    if (new_challenge) {
        new_challenge->text = NULL;
        new_challenge->word = NULL;
    }
    return new_challenge;
}

void chlng_reset(chlng_t *challenge) {
    if (challenge) {
        free(challenge->text);
        free(challenge->word);
        challenge->text = NULL;
        challenge->word = NULL;
    }
}

void chlng_del(chlng_t *challenge) {
    if (challenge) {
        free(challenge->text);
        free(challenge->word);
        free(challenge);
    }
}

int chlng_fetch_text(chlng_t *challenge) {
    if (challenge) {
        FILE *fp;
        fp = popen("/usr/games/fortune", "r"); // Runs the 'fortune' command
        if (fp == NULL) {
            perror("popen");
            return -1; // Error running 'fortune'
        }

        char buffer[1024];
        fgets(buffer, sizeof(buffer), fp);
        pclose(fp);

        challenge->text = strdup(buffer);
        if (challenge->text == NULL) {
            perror("strdup");
            return -1; // Error duplicating text
        }
        return 0; // Successfully fetched text
    }
    return -1; // Invalid challenge object
}

int chlng_hide_word(chlng_t *challenge) {
    if (challenge && challenge->text) {
        char *word = strtok(challenge->text, " \t\n");
        while (word != NULL) {
            if (strlen(word) > 3) { // Hides words longer than 3 characters
                challenge->word = strdup(word);
                memset(word, '_', strlen(word));
                if (challenge->word == NULL) {
                    perror("strdup");
                    return -1; // Error duplicating word
                }
                fprintf(stderr, "%s", challenge->word);
                return 0; // Successfully hid word
            }
            word = strtok(word, " \t\n");
        }
    }
    return -1; // Invalid challenge object or no suitable word found
}