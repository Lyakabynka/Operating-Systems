#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "player.h"
#include <fcntl.h>
#include <event2/event.h>
#include "tcp.h"

static clnt_t* clients = NULL;

void clnt_del(clnt_t *me)
{
    clnt_t *clnt;
    int cfd = me->fd;

    event_del(me->event);
    (void)tcp_close(me->fd);
    if (me == clients)
    {
        clients = me->next;
        (void)free(me);
    }
    else
    {
        for (clnt = clients; clnt && clnt->next != me; clnt = clnt->next)
            ;

        if (clnt->next == me)
        {
            clnt->next = me->next;
            (void)free(me);
        }
    }
    clnt_bcast("server: clnt-%d left\n", cfd);
}

void clnt_bcast(const char *format, ...)
{
    va_list ap;
    char buf[1024];
    int len, rc;
    clnt_t *clnt, *gone = NULL;

    va_start(ap, format);
    len = vsnprintf(buf, sizeof(buf), format, ap);
    if (len > 0)
    {
        for (clnt = clients; clnt; clnt = clnt->next)
        {
            rc = tcp_write(clnt->fd, buf, len);
            if (rc <= 0)
                gone = clnt;
        }
    }
    if (gone)
        clnt_del(gone);
    va_end(ap);
}

clnt_t *clnt_new(void)
{
    clnt_t *clnt;

    clnt = calloc(1, sizeof(clnt_t));
    if (!clnt)
    {
        perror("calloc");
        return NULL;
    }
    clnt->next = clients;
    clients = clnt;
    return clnt;
}

void clnt_join(evutil_socket_t evfd, short evwhat, void *evarg)
{
    int cfd;
    clnt_t *clnt;
    struct event_base *evb = evarg;

    (void)evwhat;

    cfd = tcp_accept(evfd);
    if (cfd == -1)
    {
        return;
    }
    (void)fcntl(cfd, F_SETFL, O_NONBLOCK);
    clnt = clnt_new();
    if (!clnt)
    {
        return;
    }

    clnt->player_info = player_new();
    if (!clnt->player_info)
    {
        return EXIT_FAILURE;
    }

    clnt->fd = cfd;
    clnt->event = event_new(evb, cfd, EV_READ | EV_PERSIST, clnt_read, clnt);
    (void)event_add(clnt->event, NULL);

    clnt_bcast("server: clnt-%d joined the game!\n", cfd);

    char* msg;
    int rc = player_get_greeting(clnt->player_info, &msg);
    if (rc > 0)
    {
        tcp_write(evfd, msg, strlen(msg));
        free(msg);
    }
}

void clnt_read(evutil_socket_t evfd, short evwhat, void *evarg)
{
    char buf[1024];
    int len;
    clnt_t *me = evarg;

    (void)evwhat;

    len = tcp_read(evfd, buf, sizeof(buf));
    if (len <= 0)
    {
        clnt_del(me);
        return;
    }

    player_t *p = me->player_info;
    char *msg;
    int rc, wc;

    rc = player_get_challenge(p, &msg);
    if (rc <= 0)
    {
        perror("Unable to fetch player challenge. Aborting...");
        player_del(p);
        clnt_del(me);
        return;
    }

    wc = tcp_write(evfd, msg, strlen(msg));
    if(wc < 0)
    {
        perror("Unable to write to fd");
        free(msg);
        return;
    }
    free(msg);

    if (tcp_read(evfd, buf, sizeof(buf)) <= 0)
    {
        rc = player_post_challenge(p, "Q:", &msg);
        if (rc > 0)
        {
            tcp_write(evfd,msg, strlen(msg));
            free(msg);
        }
        return;
    }

    rc = player_post_challenge(p, buf, &msg);
    if (rc > 0)
    {
        tcp_write(evfd,msg, strlen(msg));
        free(msg);
    }

    clnt_bcast("clnt-%d: %.*s", evfd, len, buf);
}