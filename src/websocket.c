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
    char uuid[36];
};

bool is_valid_uuid(char* uuid) {
    if(strlen(uuid) != 36){
        return false;
    }
    FILE* file = fopen("/etc/gfx2linux.uuids", "r");
    if(file == NULL){
        return false;
    }
    char line[37]; // +1 for \n
    while (fgets(line, sizeof(line), file)) {
        if(strncmp(line, uuid, 36) == 0){
            return true;
        }
    }
    fclose(file);
    return false;
}

void add_uuid(char* uuid) {
    if(strlen(uuid) != 36){
        return;
    }
    FILE* file = fopen("/etc/gfx2linux.uuids", "a");
    fprintf(file, "%s\n", uuid);
    fclose(file);
}
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
            puts(in);
            if(!pss->auth){
                if (strncmp(in, "uuid:\t", 6) == 0){
                    strncpy(pss->uuid, in+6, 36);
                    if(is_valid_uuid(pss->uuid)){
                        pss->auth = true;
                        char* msg = "AUTH:OK";
                        lws_write(wsi, (unsigned char*)strdup(msg), strlen(msg), LWS_WRITE_TEXT);
                    }
                } else if (strncmp(in, "pin:\t", 5) == 0){
                    char *msg;
                    if (strncmp(in+5, pss->buf, 6) == 0){
                        pss->auth = true;
                        add_uuid(pss->uuid);
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
        case LWS_CALLBACK_HTTP:
             char *requested_uri = (char *) in;
             while(requested_uri[0] == '/'){
                 requested_uri++;
             }
             printf("requested URI: %s\n", requested_uri);
             char* response =  get_response(requested_uri);

             lws_write(wsi, (unsigned char *)response, strlen(response), LWS_WRITE_HTTP);
             return -1;
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
