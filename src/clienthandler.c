#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../headers/content-type.h"
#include "../headers/clienthandler.h"
#define BUFFER_SIZE 1024


void* clienthandler(void* arg){

    ClientArgs* clientArgs = (ClientArgs*)arg;
    int client_socket = clientArgs->client_socket;
  

     
        
        char buffer[BUFFER_SIZE] = {0};
        
        if(recv(client_socket, buffer, BUFFER_SIZE, 0)<=0){
            printf("Veri alinamadi veya baglanti kapandi\n");
            close(client_socket);
            free(clientArgs);
            
            return NULL;
        }

        char *line = strstr(buffer, "GET ");
        
        if(line!=NULL){
            char *start = line+4;

            char *end = strstr(start, " HTTP/");
            if(end!=NULL){
                int length = end-start;

                char filename[255] = {0};
                
                strncpy(filename,start,length);

                filename[length]='\0';
                 if (strstr(filename, "..") != NULL) {
                    char *forbidden = "HTTP/1.1 403 Forbidden\r\n\r\nErisim Reddedildi!";
                    send(client_socket, forbidden, strlen(forbidden), 0);
                    close(client_socket);
                    free(clientArgs);
                    return NULL;
                }


                if((filename[0]=='/')&&(filename[1]=='\0')){
                    strcpy(filename,"/index.html");
                }
                printf("----------------------------------\n");
                
                printf("ISTENEN DOSYA: %s-< \n",filename);
                printf("----------------------------\n");
                printf("DEBUG: filename icerigi: [%s]\n", filename);
                printf("DEBUG: filename+1 icerigi: [%s]\n", filename + 1);
                char path[255] = "public/";
                strcat(path,filename+1);
                printf("DEBUG: path icerigi: [%s]\n", path);
               
                FILE* file = fopen((path),"rb");
                if(file==NULL){
                    char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
                    send(client_socket, not_found, strlen(not_found), 0);
                    free(clientArgs);
                    close(client_socket);
                    
                    return NULL;
                }
                fseek(file,0,SEEK_END);

                long fsize = ftell(file);

                fseek(file,0,SEEK_SET);

                char *file_content = (char*)malloc(fsize+1);
                if(!file_content){
                    fclose(file);
                    char *not_found = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\n\r\nInternal Server Error";
                    send(client_socket, not_found, strlen(not_found), 0);
                    free(clientArgs);
                    close(client_socket);
                    return NULL;
                }

                fread(file_content,1,fsize,file);
                file_content[fsize] = '\0';

                char response_header[BUFFER_SIZE];
                
                printf("DOSYA ICERIGI: %s\n",file_content);
                
                const char* content_type = get_content_type(filename);

                sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n", fsize, content_type);
                send(client_socket, response_header, strlen(response_header), 0);
                send(client_socket, file_content, fsize, 0);
                free(file_content);
                fclose(file);
                


            }

        }
            close(client_socket);
            free(clientArgs);
           
            return NULL;


}