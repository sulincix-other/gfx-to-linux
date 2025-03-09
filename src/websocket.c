#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define BUFFER_LEN 1024
#define WEBSOCKET_PORT 8080

#include "gfx2linux.h"

struct per_session_data {
    int fd;
    bool auth;
    char buf[BUFFER_LEN];
    
};

static int callback_echo(struct lws *wsi, 
                          enum lws_callback_reasons reason, 
                          void *user, 
                          void *in, 
                          size_t len) {
    struct per_session_data *pss = (struct per_session_data *)user;
    (void)len;
    srand(time(NULL));

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            printf("Connection established\n");
            int pin = rand() % 1000000;
            sprintf(pss->buf, "%06d\n", pin);
            pss->auth = false;
            printf("Your random PIN code is: %s\n", pss->buf);
            char message[2048];
            sprintf(message,"name:\t%s\nsum:\t%s\nbody:\t%s\n", "Input Request", "PIN Code", pss->buf);
            send_message_users(message);
            break;

        case LWS_CALLBACK_RECEIVE:
            if(!pss->auth){
                if (strncmp(in, "pin:\t", 5) == 0){
                    char *msg;
                    if (strncmp(in+5, pss->buf, 6) == 0){
                        pss->auth = true;
                        msg = "AUTH:OK";
                    } else {
                        msg = "AUTH:ERR";
                    }
                    lws_write(wsi, (unsigned char*)strdup(msg), strlen(msg), LWS_WRITE_TEXT);
                }
                break;
            }
            printf("Received: %s\n", (char *)in);
            Event ev = parse_data((char*)in);
            printf("%s\n",pss->buf);
            uinput_event(ev);
            //lws_write(wsi, (unsigned char *)in, len, LWS_WRITE_TEXT);
            break;

        case LWS_CALLBACK_CLOSED:
            printf("Connection closed\n");
            break;

        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "echo-protocol",
        callback_echo,
        sizeof(struct per_session_data),
        BUFFER_LEN,
    },
    { NULL, NULL, 0, 0 }
};

int websocket_init() {
    struct lws_context_creation_info info;
    struct lws_context *context;

    memset(&info, 0, sizeof(info));
    info.port = WEBSOCKET_PORT;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "lws_create_context failed\n");
        return -1;
    }

    while (1) {
        lws_service(context, 0);
    }

    lws_context_destroy(context);
    return 0;
}
