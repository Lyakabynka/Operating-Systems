#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "player.h"

player_t *player_new() {
    player_t *new_player = (player_t *)malloc(sizeof(player_t));
    if (new_player) {
        new_player->solved = 0;
        new_player->total = 0;
        new_player->finished = false;
        new_player->chlng = chlng_new();
    }
    return new_player;
}

void player_reset(player_t *player) {
    player->solved = 0;
    player->total = 0;
    player->finished = false;
    chlng_reset(player->chlng);
}

void player_del(player_t *player) {
    if (player) {
        chlng_del(player->chlng);
        free(player);
    }
}

int player_fetch_chlng(player_t *player) {
    if (player) {
        int rc = chlng_fetch_text(player->chlng);
        if (rc != 0) {
            return -1; // Error fetching challenge
        }
        rc = chlng_hide_word(player->chlng);
        if (rc != 0) {
            return -1; // Error hiding word
        }
        player->total++;
        return 1; // Challenge fetched successfully
    }
    return -1; // Invalid player
}

int player_get_greeting(player_t *player, char **greeting) {
    if (player) {
        *greeting = strdup("M: Welcome to the word guessing game!\n");
        return strlen(*greeting);
    }
    return -1; // Invalid player
}

int player_get_challenge(player_t *player, char **challenge) {
    if (player) {
        if (player_fetch_chlng(player) < 0) {
            return -1; // Error fetching challenge
        }
        *challenge = strdup("C: Guess the missing word in the phrase!\n");
        char *hidden_word = player->chlng->text;
        // Code to create challenge message with hidden word (player->chlng->word)
        // Append it to the challenge message (use sprintf, strcat, etc.)
        strcat(*challenge, "The phrase: ");
        strcat(*challenge, hidden_word);
        strcat(*challenge, "\n");
        strcat(*challenge, "R: Send your guess in the form 'R:word\\r\\n'.\n");
        return strlen(*challenge);
    }
    return -1; // Invalid player
}

int player_post_challenge(player_t *player, char *line, char **response) {
    if (player) {
        if (strncmp(line, "R:", 2) != 0) {
            *response = strdup("M: Invalid format. Send your guess in the form 'R:word\\r\\n'.\n");
            return strlen(*response);
        }

        // Extract guess from 'R:word'
        char *guess = strtok(line + 2, "\r\n");

        if (strcmp(guess, player->chlng->word) == 0) {
            player->solved++;
            *response = strdup("O: Congratulations! Challenge passed!\n");
        } else {
            *response = strdup("F: Wrong guess. Try again!\n");
        }
        return strlen(*response);
    }
    return -1; // Invalid player
}