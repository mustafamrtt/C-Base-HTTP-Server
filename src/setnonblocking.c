#include "../headers/setnonblocking.h"

int setnonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}