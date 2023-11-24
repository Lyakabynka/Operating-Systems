#ifndef _CLIENT_H
#define _CLIENT_H

#include "player.h"
#include <event2/event.h>

typedef struct clnt {
    evutil_socket_t   fd;
    struct event      *event;
    struct clnt       *next;
    struct player_t   *player_info;
} clnt_t;

clnt_t* clnt_new(void);
void clnt_del(clnt_t *clnt);
void clnt_bcast(const char *format, ...);

void clnt_join(evutil_socket_t evfd, short evwhat, void *evarg);
void clnt_read(evutil_socket_t evfd, short evwhat, void *evarg);

#endif