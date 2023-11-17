#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "player.h"
#include "tcp.h"

#define PORT "12345" // Change to your desired port

int main() {
    int sockfd_ipv4, sockfd_ipv6, maxfd, clientfd;
    player_t *player;
    char *msg;
    int rc;

    sockfd_ipv4 = tcp_listen(NULL, PORT); // Listening socket for IPv4
    if (sockfd_ipv4 < 0) {
        perror("tcp_listen IPv4");
        return EXIT_FAILURE;
    }

    sockfd_ipv6 = tcp_listen("::", PORT); // Listening socket for IPv6
    if (sockfd_ipv6 < 0) {
        perror("tcp_listen IPv6");
        return EXIT_FAILURE;
    }

    maxfd = (sockfd_ipv4 > sockfd_ipv6) ? sockfd_ipv4 : sockfd_ipv6;

    fd_set masterfds, readfds;
    FD_ZERO(&masterfds);
    FD_SET(sockfd_ipv4, &masterfds);
    FD_SET(sockfd_ipv6, &masterfds);

    while (1) {
        readfds = masterfds;
        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        for (int fd = 0; fd <= maxfd; fd++) {
            if (FD_ISSET(fd, &readfds)) {
                if (fd == sockfd_ipv4 || fd == sockfd_ipv6) {
                    clientfd = tcp_accept(fd); // Accept incoming connection
                    if (clientfd < 0) {
                        perror("tcp_accept");
                        continue;
                    }

                    player = player_new(); // Initialize player
                    if (!player) {
                        perror("player_new");
                        return EXIT_FAILURE;
                    }

                    while (!(player->finished)) {
                        rc = player_get_challenge(player, &msg); // Get challenge message
                        if (rc > 0) {
                            tcp_write(clientfd, msg, strlen(msg)); // Send challenge message to client
                            free(msg);
                        }

                        char line[1024];
                        rc = tcp_read(clientfd, line, sizeof(line)); // Read client response
                        if (rc <= 0) {
                            perror("tcp_read");
                            break;
                        }
                        line[rc] = '\0';

                        rc = player_post_challenge(player, line, &msg); // Post challenge and get response
                        if (rc > 0) {
                            tcp_write(clientfd, msg, strlen(msg)); // Send response to client
                            free(msg);
                        }
                    }

                    player_del(player); // Clean up player resources
                    tcp_close(clientfd); // Close client socket
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
