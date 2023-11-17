#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "player.h"
#include "chlng.h"

player_t* player_new() {
    player_t* new_player = (player_t*)malloc(sizeof(player_t));
    if (new_player != NULL) {
        new_player->solved = 0;
        new_player->total = 0;
        new_player->finished = false;
        new_player->chlng = chlng_new();
    }
    return new_player;
}

void player_reset(player_t* player) {
    if (player != NULL) {
        player->solved = 0;
        player->total = 0;
        player->finished = false;
        chlng_reset(player->chlng);
    }
}

void player_del(player_t* player) {
    if (player != NULL) {
        chlng_del(player->chlng);
        free(player);
    }
}

int player_fetch_chlng(player_t* player) {
    if (player->chlng != NULL) {
        int fetch_result = chlng_fetch_text(player->chlng);
        if (fetch_result > 0) {
            int hide_result = chlng_hide_word(player->chlng);
            if (hide_result > 0) {
                return 1; // Challenge fetched successfully
            }
        }
    }
    return -1; // Error handling
}

int player_get_greeting(player_t* player, char** msg) {
    *msg = strdup("M: Welcome to the word guessing game!\n");
    if (*msg != NULL) {
        return strlen(*msg); // Return length of message
    }
    return -1; // Error handling
}

int player_get_challenge(player_t* player, char** msg) {
    if (player->chlng != NULL) {
        *msg = malloc(strlen("C: Guess the missing word: ") + strlen(player->chlng->text) + 1);
        if (*msg != NULL) {
            sprintf(*msg, "C: Guess the missing word: %s\n", player->chlng->text);
            return strlen(*msg); // Return length of message
        }
    }
    return -1; // Error handling
}


int player_post_challenge(player_t* player, char* guess, char** msg) {
    if (player->chlng != NULL && player->chlng->word != NULL && guess != NULL) {
        if (strcmp(guess, player->chlng->word) == 0) {
            player->solved++;
            *msg = strdup("O: Congratulations - challenge passed!\n");
        } else {
            *msg = strdup("F: Wrong guess - expected a different word.\n");
        }
        if (*msg != NULL) {
            player->total++;
            return strlen(*msg); // Return length of message
        }
    }
    return -1; // Error handling
}