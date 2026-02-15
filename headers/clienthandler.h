#ifndef clienthandler_h
#define clienthandler_h
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
typedef struct ClientArgs{
    int client_socket;
    int server_fd;
    int addrlen;
    
    struct sockaddr_in address;
}ClientArgs;
void* clienthandler(void* arg);
static bool has_keep_alive(const char *buf,size_t len);
#endif 