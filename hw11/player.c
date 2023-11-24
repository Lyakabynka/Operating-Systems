#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

player_t *player_new(void) {
    player_t *new_player = (player_t *)malloc(sizeof(player_t));
    if (new_player == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_player->solved = 0;
    new_player->total = 0;
    new_player->finished = false;
    new_player->chlng = chlng_new();
    return new_player;
}

void player_reset(player_t *p) {
    if (p) {
        p->solved = 0;
        p->total = 0;
        p->finished = false;
        chlng_reset(p->chlng);
    }
}

void player_del(player_t *p) {
    if (p) {
        chlng_del(p->chlng);
        free(p);
    }
}

int player_fetch_chlng(player_t *p) {
    if (p == NULL || p->chlng == NULL) {
        return -1;  // Invalid player or challenge
    }
    int status = chlng_fetch_text(p->chlng);
    if (status == 1) {
        status = chlng_hide_word(p->chlng);
        if (status == 1) {
            return 1;  // Challenge fetched and word hidden successfully
        }
    }
    return -1;  // Failed to fetch challenge
}

int player_get_greeting(player_t *p, char **msg) {
    if (p == NULL || msg == NULL) {
        return -1;  // Invalid player or message pointer
    }
    *msg = strdup("M: Greetings! Guess the missing word in the phrase.\nM: Send your guess in the form 'R:word\\r\\n'.\n");
    if (*msg == NULL) {
        perror("Memory allocation failed");
        return -1;
    }
    return strlen(*msg);
}

int player_get_challenge(player_t *p, char **msg) {
    if (p == NULL || p->chlng == NULL || msg == NULL) {
        return -1;  // Invalid player, challenge, or message pointer
    }
    *msg = strdup("C: Guess the missing word in this phrase: ");
    if (*msg == NULL) {
        perror("Memory allocation failed");
        return -1;
    }

    if(player_fetch_chlng(p) < 0)
    {
        perror("Error fetcing challenge from fortune");
        return -1;
    } 

    fprintf(stderr, p->chlng->text);

    strcat(*msg, p->chlng->text);
    strcat(*msg, "\n");
    return strlen(*msg);
}

int player_post_challenge(player_t *p, char *input, char **response) {
    if (p == NULL || p->chlng == NULL || input == NULL || response == NULL) {
        return -1;  // Invalid player, challenge, input, or response pointer
    }
    char *word = strstr(input, "R:");
    if (word != NULL) {
        word += 2;  // Move past 'R:'
        word[strcspn(word, "\r\n")] = '\0';  // Remove newline or carriage return
        if (strcmp(word, p->chlng->word) == 0) {
            *response = strdup("O: Congratulation - challenge passed!\n");
            if (*response == NULL) {
                perror("Memory allocation failed");
                return -1;
            }
            p->solved++;
            p->total++;
            return strlen(*response);
        } else {
            *response = strdup("F: Wrong guess - expected '");
            if (*response == NULL) {
                perror("Memory allocation failed");
                return -1;
            }
            strcat(*response, p->chlng->word);
            strcat(*response, "'\n");
            p->total++;
            return strlen(*response);
        }
    } else if (strncmp(input, "Q:", 2) == 0) {
        p->finished = true;
        *response = strdup("M: You quit. Goodbye!\n");
        if (*response == NULL) {
            perror("Memory allocation failed");
            return -1;
        }
        return strlen(*response);
    } else {
        *response = strdup("M: Invalid input. Send your guess in the form 'R:word\\r\\n' or 'Q:' to quit.\n");
        if (*response == NULL) {
            perror("Memory allocation failed");
            return -1;
        }
        return strlen(*response);
    }
}
