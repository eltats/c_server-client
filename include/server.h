#ifndef SERVER_H
#define SERVER_H
// #define _GNU_SOURCE /* See feature_test_macros(7) */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

typedef const struct sockaddr *s_saddr;

#define TRUE  1
#define FALSE 0

#define DEF_SERVER_PATH "/tmp/C_SERVER.sock"
#define DEF_BACKLOG     1
#define DEF_MAX_CLIENTS 3

void server_new_connection(int fd_server, int fd_clients[], int *avail_clients);
void server_read_from_client(int *fd_client, int *avail_clients);

void fdsel_reinit(fd_set *fds, int *fds_max, int fd_server, int fd_clients[]);
void fdsel_update_maxfd(int *fd_max, int fd_new);

void hand_sigint(int s);
void hand_err(int res, const char *fn);

#endif