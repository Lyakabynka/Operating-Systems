#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "player.h"



int main(void)
{
    struct event_base *evb;
    struct event *ev;
    listen_t *iface, interfaces[] = {
                         {.address = "0.0.0.0"}, /* IPv4 any address */
                         {.address = "::"},      /* IPv6 any address */
                         {.address = NULL}       /* end marker */
                     };

    if (argc != 2)
    {
        usage(stderr, EXIT_FAILURE);
    }

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        perror("signal");
        return EXIT_FAILURE;
    }

    evb = event_base_new();
    if (!evb)
    {
        fprintf(stderr, "event_base_new: failed\n");
        return EXIT_FAILURE;
    }
    for (iface = interfaces; iface->address; iface++)
    {
        iface->fd = tcp_listen(iface->address, argv[1]);
        if (iface->fd == -1)
        {
            continue;
        }
        ev = event_new(evb, iface->fd, EV_READ | EV_PERSIST, clnt_join, evb);
        event_add(ev, NULL);
    }
    if (event_base_loop(evb, 0) == -1)
    {
        fprintf(stderr, "event_base_loop: failed\n");
        event_base_free(evb);
        return EXIT_FAILURE;
    }
    (void)event_base_free(evb);
    return EXIT_SUCCESS;
}