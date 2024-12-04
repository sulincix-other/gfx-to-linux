#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "gfx2linux.h"


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
            }if(startswith(token, "X:")){
                ev->x = atoi(token+2);
            }if(startswith(token, "Y:")){
                ev->y = atoi(token+2);
            }
        }
        token = strtok(NULL, "\n");
    }
    return ev[0];
}