#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <getopt.h>

#include "tcp.h"
#include "player.h"
#include "chlng.h"

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


    while (1) {
        fd_set fdset;
        FD_ZERO(&fdset);
        int maxfd = 0;
        for (listen_t *l = listeners; l->address; l++) {
            if (l->fd > 0) {
                FD_SET(l->fd, &fdset);
                maxfd = (l->fd > maxfd) ? l->fd : maxfd;
            }
        }
        if (echo == echo_event) {
            for (int i = 0; i < no_clients; i++) {
                FD_SET(cfds[i], &fdset);
                maxfd = (cfds[i] > maxfd) ? cfds[i] : maxfd;
            }
        }
        if (maxfd == 0) {
            break;
        }
        if (select(1 + maxfd, &fdset, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
	}
        for (listen_t *l = listeners; l->address; l++) {
            if (l->fd > 0 && FD_ISSET(l->fd, &fdset)) {                
                /*
                 * Note that we let the parent process / main thread
                 * accept the new connection.  If we let the child
                 * process / thread do this, then we can run into the
                 * situation that the subsequent select may see the
                 * socket as still readable (in case the process /
                 * thread did not yet run to accept from the socket)
                 * and then the main loop may create yet another child
                 * process / thread.
                 */
                int cfd = tcp_accept(l->fd);
                if (cfd == -1) {
                    perror("accept");
                } else {
                    if (echo == echo_event) {
                        if (no_clients+1 < CFDS_SIZE) {
                            cfds[no_clients++] = cfd;
                        } else {
                            fprintf(stderr, "too many clients\n");
                            close(cfd);
                        }
                    } else {
                        echo(cfd);
                    }
                }
            }
        }
        if (echo == echo_event) {
            for (int i = 0; i < no_clients; i++) {
                if (FD_ISSET(cfds[i], &fdset)) {
                    int rc = tcp_read_write(cfds[i], cfds[i]);
                    if (rc == 0) {
                        close(cfds[i]);
                        memmove(cfds+i, cfds+(i+1),
                                (no_clients-i) * sizeof(int));
                        no_clients--;
                    }
                }
            }
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