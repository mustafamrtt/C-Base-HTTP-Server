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
<<<<<<< HEAD
=======
#include "../headers/setnonblocking.h"
#include <sys/epoll.h>


>>>>>>> feature/epoll
#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10
struct epoll_event ev, events[MAX_EVENTS];
const size_t num_threads = 4;



const size_t num_threads = 4;



int main(){

    pthread_t thread_id;

    
   
    

    int server_fd, new_socket,nfds,epollfd;
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
<<<<<<< HEAD
    int *vals;
    

    tm = tpool_create(num_threads);

   
=======
   
    
    epollfd = epoll_create1(0);
    if(epollfd == -1){
        perror("epoll_create1 error");
        return -1;
    } 
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;


    tm = tpool_create(num_threads);

   if(epoll_ctl(epollfd, EPOLL_CTL_ADD,server_fd,&ev)==-1){
        perror("epoll_ctl error");
        return -1;
   }
>>>>>>> feature/epoll
    
    while(1){

        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

        for(int i=0;i<nfds;i++){
            if(events[i].data.fd == server_fd){
                if((new_socket = accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&addrlen))<0){
                    perror("accept error");
                    continue;
                }
                ClientArgs* clientArgs = (ClientArgs*)malloc(sizeof(ClientArgs));
                clientArgs->server_fd = server_fd;
                clientArgs->addrlen = addrlen;
                clientArgs->address = address;
                clientArgs->client_socket = new_socket;
                setnonblocking(new_socket);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = new_socket;
                if(epoll_ctl(epollfd,EPOLL_CTL_ADD,new_socket,&ev)==-1){
                    perror("epoll_ctl error");
                    return -1;
                }
                tpool_add_work(tm,(thread_func_t)(clienthandler), (void*)clientArgs);

            }
        }
<<<<<<< HEAD
        ClientArgs* clientArgs = (ClientArgs*)malloc(sizeof(ClientArgs));
        clientArgs->server_fd = server_fd;
        clientArgs->addrlen = addrlen;
        clientArgs->address = address;
        clientArgs->client_socket = new_socket;



       
        tpool_add_work(tm,(thread_func_t)(clienthandler), (void*)clientArgs);




       
    
=======
        
>>>>>>> feature/epoll
    }
   
    
    close(server_fd);
    tpool_destroy(tm);
    return 0;
}