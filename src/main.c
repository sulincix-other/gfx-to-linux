#include <pthread.h>

#include "gfx2linux.h"

int main(){
    // uinput init
    uinput_init();
    // run web server
    return websocket_init();
}
