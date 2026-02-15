#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../headers/content-type.h"
#include "../headers/clienthandler.h"
#define BUFFER_SIZE 4096


int parsefind(char* buffer,size_t len){
    if(len<4){
        return 0;
    }
    for(int i=3;i<len;i++){
        if(buffer[i-3]=='\r'&&buffer[i-2]=='\n'&&buffer[i-1]=='\r'&&buffer[i]=='\n')
            return i+1;
    }
    return 0;
}

void* clienthandler(void* arg){

    ClientArgs* clientArgs = (ClientArgs*)arg;
    int client_socket = clientArgs->client_socket;
    char buffer[BUFFER_SIZE] = {0};
    
    int size=0;
        while(1){
           int bytes = recv(client_socket,buffer+size,BUFFER_SIZE-size,0);
           if(bytes<=0){
            break;
           }
           size+=bytes;
           if(parsefind(buffer,size)){
            break;
           }
            
           
           if(size>=BUFFER_SIZE){
            char* bad_request = "HTTP/1.1 400 Bad Request\r\nContent-Length: 11\r\n\r\nBad Request";
            send(client_socket, bad_request, strlen(bad_request), 0);
            close(client_socket);
            free(clientArgs);
            return NULL;
           }
        }
       buffer[size]='\0';
        
        
       
        
      

        int header_length = parsefind(buffer,size);

        
        

        
        if(header_length>0){
            
           

          
            if(strncmp(buffer,"GET", 3)==0){
                char* line = strstr(buffer,"GET");
                char *start = line+4;
                char *end = strstr(start, " HTTP/");
                if(end!=NULL){
                    int length = end-start;
                    char filename[255] = {0};
                    strncpy(filename,start,length);
                    filename[length]='\0';
                     if (strstr(filename, "..") != NULL) {
                        char *forbidden = "HTTP/1.1 403 Forbidden\r\n\r\nConnection refused!";
                        send(client_socket, forbidden, strlen(forbidden), 0);
                        close(client_socket);
                        free(clientArgs);
                        return NULL;
                    }
                    if(filename[0]=='/'&&filename[1]=='\0'){
                        strcpy(filename,"index.html");
                    }
                    printf("----------------------------------\n");
                    printf("FILE: %s-< \n",filename);
                    printf("----------------------------\n");   
                    char path[255] = "../public/";
                    strcat(path,filename+1);
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
                    printf("File Content: %s\n",file_content);
                    const char* content_type = get_content_type(filename);
                    sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n", fsize, content_type);
                    send(client_socket, response_header, strlen(response_header), 0);
                    send(client_socket,file_content,fsize,0);
                    fclose(file);
                    free(file_content);
                    
                    close(client_socket);
                    
                }
           
             
            }
            else if(strncmp(buffer,"POST",4)==0){
                if(buffer[header_length-1]=='\n'&&buffer[header_length-2]=='\r'&&buffer[header_length-3]=='\n'&&buffer[header_length-4]=='\r'){
                    char *ok_response = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
                    send(client_socket, ok_response, strlen(ok_response), 0);
                }
               char path[256];
               sscanf(buffer, "%*s %s",path);
               printf("Path: %s\n",path);
               buffer[size]='\0';
               printf("POST header: %s\n",buffer);
               char* line = strstr(buffer,"Content-Length:");

               int content_length = atoi(line+15);
               printf("Content-Length: %d\n",content_length);

               
               int header_length = parsefind(buffer,size);
               
               char* body_start = buffer+header_length;
               int bodyinbuffer = size-header_length;

               int remaining = content_length-bodyinbuffer;
               while(remaining){
                int bytes  = recv(client_socket,buffer+size,BUFFER_SIZE-size-1,0);

                if(bytes<=0) break;

                size+=bytes;

                remaining -=bytes;


               }
               char* body = buffer+header_length;

               body[content_length]='\0';
               if(strcmp(path,"/submit")==0){
               
               printf("POST body: %s\n",body);

               char* pair = strtok(body,"&");
               while(pair!=NULL){
                char* key = strtok(pair,"=");

                char* value = strtok(NULL,"=");

                if(key&&value){
                    printf("Key: %s, Value: %s\n",key,value);
                }
                   pair = strtok(NULL,"&");

            }
         
              
        }
               
               

        
            close(client_socket);
            free(clientArgs);
           
            return NULL;

        
            }

            else {
                char* bad_request = "HTTP/1.1 400 Bad Request\r\nContent-Length: 11\r\n\r\nBad Request";
                send(client_socket, bad_request, strlen(bad_request), 0);
                close(client_socket);
                free(clientArgs);
                
            }

        }
}
