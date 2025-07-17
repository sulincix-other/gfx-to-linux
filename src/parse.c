#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <linux/uinput.h>


#include "gfx2linux.h"


Event parse_data(char* data){
    //printf("Received request:\n%s\n", data);
    char *token;
    token = strtok(data, "\n");
    Event *ev = calloc(1, sizeof(Event));
    char* val;
    while (token != NULL) {
        if(startswith(token, "type:")){
            val = strdup(token+6);
            if(startswith(val, "EV_KEY")){
                ev->type = EV_KEY;
            }else if(startswith(val, "EV_ABS")){
                ev->type = EV_ABS;
            }else if(startswith(val, "EV_REL")){
                ev->type = EV_REL;
            }
        }if(startswith(token, "code:")){
            val = strdup(token+6);
            if(startswith(val, "ABS_X")){
                ev->code = ABS_X;
            } else if(startswith(val, "ABS_Y")){
                ev->code = ABS_Y;
            } else if(startswith(val, "BTN_TOUCH")){
                ev->code = BTN_TOUCH;
            } else if(startswith(val, "BTN_LEFT")){
                ev->code = BTN_LEFT;
            } else if(startswith(val, "BTN_RIGHT")){
                ev->code = BTN_RIGHT;
            } else if(startswith(val, "REL_WHEEL_HI_RES")){
                ev->code = REL_WHEEL_HI_RES;
            } else if(startswith(val, "REL_WHEEL")){
                ev->code = REL_WHEEL;
            } else {
                ev->code = atoi(val);
            }

        }if(startswith(token, "value:")){
            ev->value = atoi(token+7);
        }
        token = strtok(NULL, "\n");
    }
    return ev[0];
}
