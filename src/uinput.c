#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/uinput.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>

#include "gfx2linux.h"

struct libevdev *dev;
struct libevdev_uinput *uidev;
struct input_absinfo absinfo_x;
struct input_absinfo absinfo_y;
int err;
void uinput_init(){
    // load module
    DIR* d = opendir("/sys/module/uinput");
    if(d == NULL){
        err = system("modprobe uinput");
        if (err != 0) { perror("uinput"); exit(err); }
    } else {
        closedir(d);
    }

    dev = libevdev_new();
    libevdev_set_name(dev, "Amogus device");

    // ABS info
    absinfo_x.maximum = 3840;
    absinfo_x.resolution = 1;
    absinfo_y.maximum = 2160;
    absinfo_y.resolution = 1;

    // EV_ABS
    libevdev_enable_event_type(dev, EV_ABS);
    libevdev_enable_event_code(dev, EV_ABS, ABS_X, &absinfo_x);
    libevdev_enable_event_code(dev, EV_ABS, ABS_Y, &absinfo_y);
    libevdev_enable_event_code(dev, EV_ABS, ABS_PRESSURE, &absinfo_y);

    // EV_REL
    libevdev_enable_event_type(dev, EV_REL);
    libevdev_enable_event_code(dev, EV_REL, REL_WHEEL, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_HWHEEL, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_WHEEL_HI_RES, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_HWHEEL_HI_RES, NULL);

    // EV_KEY (mouse)
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, BTN_RIGHT, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_TOUCH, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_LEFT, NULL);

    // EV_KEY (keyboard)
    for (int i = 1; i <= 245; i++) {
        libevdev_enable_event_code(dev, EV_KEY, i, NULL);
    }


    err = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if (err != 0) { perror("uinput"); exit(err); }

    sleep(1);
}

void send_event(int type, int code, int value) {
    //printf("%d %d %d\n", type, code, value);
	libevdev_uinput_write_event(uidev, type, code, value);
	libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
}

void uinput_event(Event ev){
    send_event(ev.type, ev.code, ev.value);
    send_event(EV_SYN, SYN_REPORT, 0);
}
