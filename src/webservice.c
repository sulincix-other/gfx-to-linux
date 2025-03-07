#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define startswith(A, B) strncmp(A, B, strlen(B)) == 0

#define PORT 8081
#define BUFFER_SIZE 10240

#include "gfx2linux.h"

extern char* page;
extern char* styles;
extern char* script;
extern char* keycodes;
void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    free(client_socket);

    char buffer[BUFFER_SIZE];
    read(sock, buffer, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0'; // Null-terminate the buffer

    // Parse the GET request
    char *method = strtok(buffer, " ");
    char *path = strtok(NULL, " ");
    
    // Check if the method is GET
    if (method == NULL || path == NULL || strcmp(method, "GET") != 0) {
        // Respond with 400 Bad Request
        char *http_response = "HTTP/1.1 400 Bad Request\nConnection: close\n\n";
        write(sock, http_response, strlen(http_response));
        close(sock);
        return NULL;
    }

    // Remove leading '/' from path
    if (path[0] == '/') {
        path++;
    }

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
        // Respond with js content
        http_response = strdup(
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/json\n"
            "Connection: close\n\n"
        );
        http_response = realloc(http_response, 
            (strlen(http_response) + strlen(keycodes) + 1)*sizeof(char)
        );
        strcat(http_response, keycodes);
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

    // Send the response
    write(sock, http_response, strlen(http_response));
    free(http_response);
    close(sock);
    return NULL;
}



int service_main() {
    int server_fd, *new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", PORT);

    while (1) {
        new_socket = malloc(sizeof(int));
        if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            free(new_socket);
            continue;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, new_socket) != 0) {
            perror("pthread_create");
            free(new_socket);
        }
        pthread_detach(thread_id); // Detach the thread to free resources when done
    }

    close(server_fd);
    return 0;
}
