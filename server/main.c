#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "parse.h"

#define PORT 8080
#define BUFFER_SIZE 10240



extern char* page;

void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    free(client_socket);

    char buffer[BUFFER_SIZE];
    read(sock, buffer, sizeof(buffer) - 1);
    Event ev = parse_data(buffer);
    // Simple HTTP response
    char *http_response = strdup( 
        "HTTP/1.1 200 OK\n"
        "Content-Type: text/html\n"
        "Connection: close\n"
        "\n");

    if (ev.buttons < 0) {
        http_response = realloc(http_response, 
            (strlen(http_response) + strlen(page) + 1)*sizeof(char)
        );
        strcat(http_response, page);
    }

    write(sock, http_response, strlen(http_response));
    close(sock);
    free(http_response);
    return NULL;
}


int main() {
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
