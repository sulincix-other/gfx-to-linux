#ifndef _gfx2linux_h
#define _gfx2linux_h

#define SOCKET_NAME "gfx2linux.sock"

typedef struct _event {
    int type;
    int code;
    int value;
} Event;

Event parse_data(char* data);

void uinput_init();
void uinput_event(Event ev);

void send_message(char* path, char* message);
void send_message_users(char* message);

int websocket_init();
char* get_response(const char* path);
#define startswith(A, B) strncmp(A, B, strlen(B)) == 0

#endif
