#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"

#define startswith(A, B) strncmp(A, B, strlen(B)) == 0

Event parse_data(char* data){
    //printf("Received request:\n%s\n", data);
    char *token;
    token = strtok(data, "\n");
    bool e = 0;
    Event *ev = calloc(1, sizeof(Event));
    ev->buttons = -1;
    while (token != NULL) {
        // skip to body
        if(strlen(token) == 1){
            e = 1;
        }
        if(e){
            if(startswith(token, "Buttons:")){
                ev->buttons = atoi(token+9);
            }if(startswith(token, "Height:")){
                ev->height = atoi(token+8);
            }if(startswith(token, "Width:")){
                ev->width = atoi(token+7);
            }if(startswith(token, "X:")){
                ev->x = atoi(token+2);
            }if(startswith(token, "Y:")){
                ev->y = atoi(token+2);
            }
        }
        token = strtok(NULL, "\n");
    }
    printf("Buttons:%d\nHeight:%d\nWidth%d\nX:%d\nY:%d\n\n",
        ev->buttons,
        ev->height,
        ev->width,
        ev->x,
        ev->y
    );
    return ev[0];
}
