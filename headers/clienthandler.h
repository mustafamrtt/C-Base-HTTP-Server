#ifndef clienthandler_h
#define clienthandler_h
#include <netinet/in.h>
#include <sys/socket.h>
typedef struct ClientArgs{
    int client_socket;
    int server_fd;
    int addrlen;
    struct sockaddr_in address;
}ClientArgs;
void* clienthandler(void* arg);

#endif 