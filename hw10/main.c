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

#define MAX_CLIENTS 1024

int main(int argc, char *argv[])
{
    srand(time(0));

    int opt;

    char *port;
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

    int ipv4_fd, ipv6_fd, max_fd;
    fd_set read_fds;
    int client_fds[MAX_CLIENTS];

    // Initialize client_fds to -1 (no client connected)
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        client_fds[i] = -1;
    }

    // Create listening sockets for IPv4 and IPv6
    ipv4_fd = tcp_listen(NULL, "8080"); // Port 8080 for IPv4
    ipv6_fd = tcp_listen("::", "8080"); // Port 8080 for IPv6

    if (ipv4_fd == -1 || ipv6_fd == -1)
    {
        fprintf(stderr, "Failed to create listening sockets.\n");
        exit(EXIT_FAILURE);
    }

    // Determine the maximum file descriptor
    max_fd = (ipv4_fd > ipv6_fd) ? ipv4_fd : ipv6_fd;

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(ipv4_fd, &read_fds);
        FD_SET(ipv6_fd, &read_fds);

        // Add connected client sockets to the set
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            if (client_fds[i] != -1)
            {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd)
                {
                    max_fd = client_fds[i];
                }
            }
        }

        // Use select to wait for activity on sockets
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Check for incoming connections on IPv4 and IPv6 sockets
        if (FD_ISSET(ipv4_fd, &read_fds))
        {
            int new_client_fd = tcp_accept(ipv4_fd);
            if (new_client_fd != -1)
            {
                // Add the new client to the client_fds array
                for (int i = 0; i < MAX_CLIENTS; ++i)
                {
                    if (client_fds[i] == -1)
                    {
                        client_fds[i] = new_client_fd;
                        break;
                    }
                }
            }
        }

        if (FD_ISSET(ipv6_fd, &read_fds))
        {
            int new_client_fd = tcp_accept(ipv6_fd);
            if (new_client_fd != -1)
            {
                // Add the new client to the client_fds array
                for (int i = 0; i < MAX_CLIENTS; ++i)
                {
                    if (client_fds[i] == -1)
                    {
                        client_fds[i] = new_client_fd;
                        break;
                    }
                }
            }
        }

        // Check for activity on client sockets
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            if (client_fds[i] != -1 && FD_ISSET(client_fds[i], &read_fds))
            {

                player_t *p;
                char *msg;
                int rc;

                p = player_new();
                if (!p)
                {
                    return EXIT_FAILURE;
                }

                rc = player_get_greeting(p, &msg);
                if (rc > 0)
                {
                    (void)fputs(msg, stdout);
                    (void)free(msg);
                }

                while (!(p->finished))
                {
                    char *line = NULL;
                    size_t linecap = 0;

                    rc = player_get_challenge(p, &msg);
                    if (rc > 0)
                    {
                        (void)fputs(msg, stdout);
                        (void)free(msg);
                    }

                    if (getline(&line, &linecap, stdin) <= 0)
                    {
                        rc = player_post_challenge(p, "Q:", &msg);
                        if (rc > 0)
                        {
                            (void)fputs(msg, stdout);
                            (void)free(msg);
                        }
                        break;
                    }

                    rc = player_post_challenge(p, line, &msg);
                    if (rc > 0)
                    {
                        (void)fputs(msg, stdout);
                        (void)free(msg);
                    }
                    (void)free(line);
                }

                player_del(p);
            }
        }
    }

    // Close listening sockets (Not reached in this example)
    tcp_close(ipv4_fd);
    tcp_close(ipv6_fd);

    return 0;
}