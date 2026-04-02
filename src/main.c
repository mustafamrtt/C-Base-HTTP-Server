#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../headers/content-type.h"
#include "../headers/clienthandler.h"
#include "../headers/TPOOL.h"
#include <sys/epoll.h>
#include "../headers/setnonblocking.h"
#include <sys/epoll.h>
#include <errno.h>
#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_EVENTS 10


const size_t num_threads = 4;







int main(){
    struct epoll_event ev, events[MAX_EVENTS];
    pthread_t thread_id;

    
   
    

    int server_fd,nfds,epollfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket creation error");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Bind failed");
        return -1;
    }

    if(listen(server_fd,3)<0){
        perror("Listen error");
        return -1;
    }

    printf("Server is listening on %d port\n",PORT);

    tpool_t *tm;

    
    epollfd = epoll_create1(0);
    if(epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    setnonblocking(server_fd);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = server_fd;

    tm = tpool_create(num_threads);

     if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
               perror("epoll_ctl: listen_sock");
               exit(EXIT_FAILURE);
           }



    
   while(1){
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if(nfds == -1){
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }

    for(int n = 0; n < nfds; n++){

        // new connection
        if(events[n].data.fd == server_fd){
            while(1){
                int client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                if(client_socket == -1){
                    if(errno == EAGAIN || errno == EWOULDBLOCK){
                        break; 
                    } else {
                        perror("accept");
                        break;
                    }
                }

                setnonblocking(client_socket);

                struct epoll_event client_ev;        
                client_ev.events = EPOLLIN | EPOLLET;
                client_ev.data.fd = client_socket;   
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, client_socket, &client_ev) == -1){
                    perror("epoll_ctl: add client socket");
                    close(client_socket);
                }
            }
        }

        else {
            int client_socket = events[n].data.fd;
    
             
            ClientArgs* clientArgs = malloc(sizeof(ClientArgs));
            clientArgs->client_socket = client_socket;
    
            epoll_ctl(epollfd, EPOLL_CTL_DEL, client_socket, NULL);
            tpool_add_work(tm, (thread_func_t)clienthandler, (void*)clientArgs);
    }
    
   }
}
   
    
    close(server_fd);
    tpool_destroy(tm);
    return 0;
}