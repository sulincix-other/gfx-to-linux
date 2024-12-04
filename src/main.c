#include "gfx2linux.h"

int main(){
    uinput_init();
    return service_main();
}
