#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gfx2linux.h"

void send_message(char* path, char* message) {
    int sock;
    struct sockaddr_un addr;

    // Create socket
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the address structure
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    send(sock, message, strlen(message), 0);

    // Close the socket
    close(sock);
}


void send_message_users(char* message) {
    struct dirent *entry;
    DIR *dp = opendir("/run/user/");

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Skip the current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full path
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s/%s", "/run/user", entry->d_name,SOCKET_NAME);
        puts(full_path);

        struct stat statbuf;
        if (stat(full_path, &statbuf) == 0) {
            // If we found the socket file, print its path
            printf("Found: %s\n", full_path);
            send_message(full_path, message);
        }
    }

    closedir(dp);
}

#ifdef notify_test
void main(){
    send_message_users("name:\tMyApp\nsum:\tExample sum\nbody:\tHello World");
}
#endif
