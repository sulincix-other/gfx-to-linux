#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dbus/dbus.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#include "gfx2linux.h"

#define BUFFER_SIZE 1024

void send_notification(const char *app_name, const char *summary, const char *body) {
    DBusConnection *connection;
    DBusError error;
    dbus_error_init( & error);

    // Connect to the session bus
    connection = dbus_bus_get(DBUS_BUS_SESSION, & error);
    if (dbus_error_is_set( & error)) {
        fprintf(stderr, "Connection Error (%s)\n", error.message);
        dbus_error_free( & error);
        return;
    }
    if (connection == NULL) {
        fprintf(stderr, "Connection is NULL\n");
        return;
    }

    // Create a new notification
    DBusMessage *message;
    message = dbus_message_new_method_call(
        "org.freedesktop.Notifications", // destination
        "/org/freedesktop/Notifications", // object path
        "org.freedesktop.Notifications", // interface
        "Notify" // method name
    );

    // Prepare arguments
    uint32_t replaces_id = 0;
    const char *app_icon = "";
    DBusMessageIter args;
    dbus_message_iter_init_append(message, & args);

    // Append arguments to the message
    dbus_message_iter_append_basic( & args, DBUS_TYPE_STRING, & app_name);
    dbus_message_iter_append_basic( & args, DBUS_TYPE_UINT32, & replaces_id);
    dbus_message_iter_append_basic( & args, DBUS_TYPE_STRING, & app_icon);
    dbus_message_iter_append_basic( & args, DBUS_TYPE_STRING, & summary);
    dbus_message_iter_append_basic( & args, DBUS_TYPE_STRING, & body);

    // Append actions (empty array)
    DBusMessageIter array_iter;
    dbus_message_iter_open_container( & args, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING, & array_iter);
    dbus_message_iter_close_container( & args, & array_iter);

    // Append hints (empty dictionary)
    DBusMessageIter dict_iter;
    dbus_message_iter_open_container( & args, DBUS_TYPE_ARRAY, "{sv}", & dict_iter);
    dbus_message_iter_close_container( & args, & dict_iter);

    // Append timeout
    int32_t timeout = 5000; // Timeout in milliseconds
    dbus_message_iter_append_basic( & args, DBUS_TYPE_INT32, & timeout);

    // Send the message
    DBusMessage *reply;
    reply = dbus_connection_send_with_reply_and_block(connection, message, -1, & error);
    if (dbus_error_is_set( & error)) {
        fprintf(stderr, "Error sending message: %s\n", error.message);
        dbus_error_free( & error);
    }

    // Clean up
    dbus_message_unref(message);
    if (reply) {
        dbus_message_unref(reply);
    }
    dbus_connection_unref(connection);
}

char SOCKET_PATH[1024];
// Function to initialize the server socket
int socket_init() {
    int server_fd;
    struct sockaddr_un server_addr;
    sprintf(SOCKET_PATH, "/run/user/%d/%s", getuid(), SOCKET_NAME);

    // Create a Unix socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset( & server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind the socket to the address
    unlink(SOCKET_PATH);
    if (bind(server_fd, (struct sockaddr *) & server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s\n", SOCKET_PATH);
    return server_fd;
}

char app_name[BUFFER_SIZE];
char summary[BUFFER_SIZE];
char body[BUFFER_SIZE];
// Function to handle communication with a client
void *client_handler(void *arg) {
    int client_fd = *(int *) arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0) {
            perror("read");
            break;
        } else if (bytes_read == 0) {
            // Connection closed by the client
            printf("Client disconnected.\n");
            break;
        }

        // Null-terminate the received message
        buffer[bytes_read] = '\0';
        printf("Received message: %s\n", buffer);
        char *token;
        char *buffer_copy = strdup(buffer);
        token = strtok(buffer_copy, "\n");
        while (token != NULL) {
            if (strncmp(token, "name:\t", 6) == 0) {
                strcpy(app_name, token + 6);
            } else if (strncmp(token, "sum:\t", 5) == 0) {
                strcpy(summary, token + 5);
            } else if (strncmp(token, "body:\t", 6) == 0) {
                strcpy(body, token + 6);
            }
            token = strtok(NULL, "\n");
        }
        send_notification(app_name, summary, body);
        free(buffer_copy);
    }

    // Clean up
    close(client_fd);
    return NULL;
}
int main(int argc, char ** argv) {

    int server_fd = socket_init();
    struct sockaddr_un client_addr;
    socklen_t client_len;

    while (1) {
        // Accept a connection
        client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *) & client_addr, & client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Create a new thread to handle the client
        pthread_t thread_id;
        if (pthread_create( & thread_id, NULL, client_handler, (void *) & client_fd) != 0) {
            perror("pthread_create");
            close(client_fd);
        } else {
            pthread_detach(thread_id);
        }
    }

    // Clean up
    close(server_fd);
    unlink(SOCKET_PATH);

    return EXIT_SUCCESS;
}
