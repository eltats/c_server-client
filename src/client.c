#include "client.h"

int main() {
    int sd, res;
#ifdef __linux__
    struct sockaddr_un serveraddr = {AF_UNIX, DEF_SERVER_PATH};
#elif __APPLE__
    struct sockaddr_un serveraddr = {sizeof(DEF_SERVER_PATH) - 1, AF_UNIX, DEF_SERVER_PATH};
#endif
    u_int32_t pid, num;

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    hand_err(sd, "socket()");

    res = connect(sd, (s_saddr)&serveraddr, sizeof(serveraddr));
    hand_err(res, "connect()");

    pid = getpid();
    printf("CLIENT: pid: [%d]\n", pid);
    // pid = htobe32(pid);

    num = 15;

    for (int i = 0; i < 5; i++) {
        res = write(sd, &pid, sizeof(u_int32_t));
        if (res == -1) {
            perror("write()");
            exit(1);
        }
        sleep(1);

        srand(time(NULL));
        num = rand();
        res = write(sd, &num, sizeof(u_int32_t));
        if (res == -1) {
            perror("write()");
            exit(1);
        }
        printf("CLIENT: pid: [%d], num: [%d]\n", pid, num);
        sleep(1);
    }
    close(sd);
    return (0);
}
