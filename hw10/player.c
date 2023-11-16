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

    return 1;
}

int player_get_greeting(player_t* p, char** msg)
{
    *msg = "Hello!";

    return 1;
}

int player_get_challenge(player_t* p, char** msg)
{
    chlng_del(p);
    
    player_fetch_chlng(p);
    *msg = p->chlng->text;

    return 1;
}

int player_post_challenge(player_t* p, char* line, char** msg)
{
    char* res;
    if(strcmp(p->chlng->word, line) == 0)
    {
        p->solved++;
        strcat(res, "Congratulation-challengepassed!");
    }
    else 
    {

        strcat(res, "Wrongguess'");
        strcat(res, line);
        strcat(res, "' - expected '");
        strcat(res, p->chlng->word);

    }

    *msg = res;

    p->total++;

    return 1;
}