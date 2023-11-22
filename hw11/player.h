#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "chlng.h"

typedef struct {
    int solved;       // Correctly solved challenges
    int total;        // Total number of challenges
    bool finished;    // True if the player has finished
    chlng_t *chlng;   // Current challenge
} player_t;

// Allocate a new player
extern player_t *player_new(void);

// Reset the internal state of the player
extern void player_reset(player_t *p);

// Delete a player and all its resources
extern void player_del(player_t *p);

// Fetch a new challenge for the player
extern int player_fetch_chlng(player_t *p);

// Retrieve a greeting message for the player
extern int player_get_greeting(player_t *p, char **msg);

// Retrieve the challenge message for the player
extern int player_get_challenge(player_t *p, char **msg);

// Post a message to the player and retrieve the response message
extern int player_post_challenge(player_t *p, char *input, char **response);

#endif
