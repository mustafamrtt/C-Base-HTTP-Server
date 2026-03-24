#ifndef clienthandler_h
#define clienthandler_h
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
typedef struct ClientArgs{
    int client_socket;
    int addrlen;
    int server_fd;
    
    struct sockaddr_in address;
}ClientArgs;
void* clienthandler(void* arg);
int parsefind(char* buffer,size_t len);
void tokenizer(char* body);
int get_method(char* buffer,int client_socket);
void post_method(char* buffer,int header_length,int client_socket,int size);
#endif 