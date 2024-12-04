#include <pthread.h>

#include "gfx2linux.h"

int main(){
    // uinput init
    uinput_init();
    // web socket init
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, (void*)service_main, NULL);
    pthread_detach(thread_id);
    // run web server
    return websocket_init();
}
