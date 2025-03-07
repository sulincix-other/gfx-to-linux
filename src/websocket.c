#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_LEN 1024
#define WEBSOCKET_PORT 8080

#include "gfx2linux.h"

struct per_session_data__echo {
    int fd;
    char buf[BUFFER_LEN];
};

static int callback_echo(struct lws *wsi, 
                          enum lws_callback_reasons reason, 
                          void *user, 
                          void *in, 
                          size_t len) {
    struct per_session_data__echo *pss = (struct per_session_data__echo *)user;
    (void)pss;

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            printf("Connection established\n");
            break;

        case LWS_CALLBACK_RECEIVE:
            printf("Received: %s\n", (char *)in);
            Event ev = parse_data((char*)in);
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
        sizeof(struct per_session_data__echo),
        BUFFER_LEN,
    },
    { NULL, NULL, 0, 0 } // terminator
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
