#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define startswith(A, B) strncmp(A, B, strlen(B)) == 0

#include "gfx2linux.h"

extern char* page;
extern char* styles;
extern char* script;
extern char* keycodes;
extern char* keyboard;


char* get_response(const char* path){
    // Simple routing based on the path
    char *http_response;
    if (strcmp(path, "styles.css") == 0) {
        // Respond with style content
        http_response = strdup(
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/css\n"
            "Connection: close\n\n"
        );
        http_response = realloc(http_response,
            (strlen(http_response) + strlen(styles) + 1)*sizeof(char)
        );
        strcat(http_response, styles);
    } else if (strcmp(path, "script.js") == 0) {
        // Respond with js content
        http_response = strdup(
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/script\n"
            "Connection: close\n\n"
        );
        http_response = realloc(http_response,
            (strlen(http_response) + strlen(script) + 1)*sizeof(char)
        );
        strcat(http_response, script);
    } else if (strcmp(path, "keycodes.json") == 0) {
        // Respond with json content
        http_response = strdup(
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/json\n"
            "Connection: close\n\n"
        );
        http_response = realloc(http_response,
            (strlen(http_response) + strlen(keycodes) + 1)*sizeof(char)
        );
        strcat(http_response, keycodes);
    } else if (strcmp(path, "keyboard.json") == 0) {
        // Respond with json content
        http_response = strdup(
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/json\n"
            "Connection: close\n\n"
        );
        http_response = realloc(http_response,
            (strlen(http_response) + strlen(keyboard) + 1)*sizeof(char)
        );
        strcat(http_response, keyboard);
    } else {
        // Respond with html content
        http_response = strdup(
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/html\n"
            "Connection: close\n\n"
        );
        http_response = realloc(http_response,
            (strlen(http_response) + strlen(page) + 1)*sizeof(char)
        );
        strcat(http_response, page);
    }
    return http_response;
}

