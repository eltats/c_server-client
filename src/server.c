#include "server.h"

// Решил выбрать select() вместо потоков, так как умные люди пишут, что это
// производительнее.
// Также использовал AF_UNIX, вместо других семейств, чтобы было меньше заморочек с
// настройкой и инициализацией.
// В целом для оптимизации можно разделять клиентские сокеты на части и слушать в
// дочерних процессах или потоках. Есть еще poll() и epoll() но в рамках тестового не имеет смысла.
// Но прочитал про все :)
int main() {
    struct sockaddr_un saddr = {AF_UNIX, DEF_SERVER_PATH};
    int                fd_server;                          // server socket descriptor
    int                fd_clients[DEF_MAX_CLIENTS] = {0};  // client socket descriptors
    int                avail_clients = DEF_MAX_CLIENTS;
    fd_set             fds_tmp;  // select() set
    int                fds_max;  // select() maxfd
    int                res;

    fd_server = socket(AF_UNIX, SOCK_STREAM, 0);
    hand_err(fd_server, "socket()");

    signal(SIGINT, hand_sigint);

    res = bind(fd_server, (s_saddr)&saddr, sizeof(saddr));
    hand_err(res, "bind()");

    res = listen(fd_server, DEF_BACKLOG);
    hand_err(res, "listen()");

    fds_max = fd_server;

    while (TRUE) {
        fdsel_reinit(&fds_tmp, &fds_max, fd_server, fd_clients);

        // check for events
        res = select(fds_max + 1, &fds_tmp, NULL, NULL, NULL);
        hand_err(res, "select()");

        // check for max available connected clients first
        // something happened on the server fd
        if (avail_clients > 0 && FD_ISSET(fd_server, &fds_tmp)) {
            server_new_connection(fd_server, fd_clients, &avail_clients);
        }

        // something happened on the client fds
        for (int i = 0; i < DEF_MAX_CLIENTS; i++) {
            if (FD_ISSET(fd_clients[i], &fds_tmp)) {
                server_read_from_client(&fd_clients[i], &avail_clients);
            }
        }
    }
    return 0;
}

void server_new_connection(int fd_server, int fd_clients[], int *avail_clients) {
    int fd_client;  // newly adopted socket descriptor

    // establish connection
    fd_client = accept(fd_server, NULL, NULL);
    if (fd_client == -1) {
        perror("accept()");
        return;
    }
    // add new socket to array of sockets 
    for (int i = 0; i < DEF_MAX_CLIENTS; i++) {
        // if position is empty 
        if (fd_clients[i] == 0) {
            fd_clients[i] = fd_client;
            printf("SERVER: new CLIENT is connected with fd: [%d]\n", i);
            *avail_clients = *avail_clients - 1;
            break;
        }
    }
}

// Не учитывал порядок байтов, так как предполагаю, что на одной машине будет запускаться.
void server_read_from_client(int *fd_client, int *avail_clients) {
    ssize_t   res;
    u_int64_t total_read, total_rest, total_shift;
    u_int64_t buf;
    u_int64_t num;

    u_int64_t size = sizeof(u_int64_t);  // how many bytes to read at a time

    num = 0;
    total_read = 0;
    total_shift = 0;
    total_rest = size;

    while (total_read < size) {
        buf = 0;

        res = read(*fd_client, &buf, total_rest);

        // error or closed connection
        if (res <= 0) {
            if (res == -1) {
                perror("read()");
            } else {
                printf("SERVER: some CLIENT is disconnected\n");
            }
            close(*fd_client);
            *fd_client = 0;
            *avail_clients = *avail_clients + 1;
            return;
        }

        num = num | buf << (total_shift * 8);
        total_shift = total_shift + res;  // increase the shift by the number of bytes read
        total_read = total_read + res;    // update the sum of bytes read
        total_rest = total_rest - res;    // update the sum of bytes remaining to read
    }

    printf("SERVER: CLIENT pid:[%d], number: [%d]\n", (int)num, (int)(num >> 32));
}

// reinit fd set for select()
// adding fd_server and clients fd to set
// update max fd if needed
void fdsel_reinit(fd_set *fds, int *fds_max, int fd_server, int fd_clients[]) {
    FD_ZERO(fds);
    FD_SET(fd_server, fds);
    fdsel_update_maxfd(fds_max, fd_server);

    // add client sockets to set 
    for (int i = 0; i < DEF_MAX_CLIENTS; i++) {
        // if valid socket descriptor then add to read list 
        if (fd_clients[i]) {
            FD_SET(fd_clients[i], fds);
        }
        fdsel_update_maxfd(fds_max, fd_clients[i]);
    }
}

void fdsel_update_maxfd(int *fd_max, int fd_new) {
    if (*fd_max && fd_new > *fd_max) {
        *fd_max = fd_new;
    }
}