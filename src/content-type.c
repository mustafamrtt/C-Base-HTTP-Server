#include <stdio.h>
#include "../headers/content-type.h"
#include <string.h>
#include <stdlib.h> 

const char* get_content_type(char* filename) {
    char* ext = strrchr(filename, '.'); 
    if (!ext) return "application/octet-stream"; 

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".pdf") == 0) return "application/pdf";
    
    return "text/plain";
}
