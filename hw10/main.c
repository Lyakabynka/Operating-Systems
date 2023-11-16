#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>

#include "tcp.h"
#include "fetch.h"

int main(int argc, char* argv[])
{
    srand(time(0));
    
    int opt;

    char* port;
    char mode = 'b';
    while ((opt = getopt(argc, argv, "ftp:")) != -1)
    {
        switch (opt)
        {
        case 'f':
        case 't':
            mode = optarg;
            break;
        case 'p':
            port = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-f] [-t] [-p port]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }


    if(mode == 'b')
    {

    }
    else if(mode == 'f')
    {
        
    }
    else 
    {

    }

    player_t *p;
    char *msg;
    int rc;

    p = player_new();
    if (!p) {
        return EXIT_FAILURE;
    }

    rc = player_get_greeting(p, &msg);
    if (rc > 0) {
        (void)fputs(msg, stdout);
        (void)free(msg);
    }

    while (!(p->finished)) {
        char *line = NULL;
        size_t linecap = 0;

        rc = player_get_challenge(p, &msg);
        if (rc > 0) {
            (void)fputs(msg, stdout);
            (void)free(msg);
        }

        if (getline(&line, &linecap, stdin) <= 0) {
            rc = player_post_challenge(p, "Q:", &msg);
            if (rc > 0) {
                (void)fputs(msg, stdout);
                (void)free(msg);
            }
            break;
        }

        rc = player_post_challenge(p, line, &msg);
        if (rc > 0) {
            (void)fputs(msg, stdout);
            (void)free(msg);
        }
        (void)free(line);
    }

    player_del(p);
    return EXIT_SUCCESS;

}