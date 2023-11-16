#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "player.h"
#include "chlng.h"

player_t* player_new(void)
{
    return malloc(sizeof(player_t));
}

void player_del(player_t* player)
{
    free(player->chlng);
    free(player);
}

int player_fetch_chnlg(player_t* player)
{
    chlng_fetch_text(player->chlng);
    chlng_hide_word(player->chlng);
}