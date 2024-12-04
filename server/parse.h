#ifndef _parse_h
#define _parse_h
typedef struct _event {
    int buttons;
    int x;
    int y;
    int width;
    int height;
} Event;

Event parse_data(char* data);
#endif
