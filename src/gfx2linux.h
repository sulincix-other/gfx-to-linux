#ifndef _gfx2linux_h
#define _gfx2linux_h
typedef struct _event {
    int buttons;
    int x;
    int y;
} Event;

Event parse_data(char* data);

int service_main();
void uinput_init();
void uinput_event(Event ev);

int websocket_init();

#define startswith(A, B) strncmp(A, B, strlen(B)) == 0

#endif
