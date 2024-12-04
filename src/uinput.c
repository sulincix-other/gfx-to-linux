#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "gfx2linux.h"

static int fd;
void uinput_init() {
    if ((fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0)
		fprintf(stderr,"error: open");
	// enable synchronization
	if (ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0)
		fprintf(stderr,"error: ioctl UI_SET_EVBIT EV_SYN");

	// enable 1 button
	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
		fprintf(stderr,"error: ioctl UI_SET_EVBIT EV_KEY");
	if (ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH) < 0)
		fprintf(stderr,"error: ioctl UI_SET_KEYBIT");
	if (ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
		fprintf(stderr,"error: ioctl UI_SET_KEYBIT");
	if (ioctl(fd, UI_SET_KEYBIT, BTN_TOOL_PEN) < 0)
		fprintf(stderr,"error: ioctl UI_SET_KEYBIT");
	if (ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS) < 0)
		fprintf(stderr,"error: ioctl UI_SET_KEYBIT");
	if (ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS2) < 0)
		fprintf(stderr,"error: ioctl UI_SET_KEYBIT");

	// enable 2 main axes + pressure (absolute positioning)
	if (ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0)
		fprintf(stderr,"error: ioctl UI_SET_EVBIT EV_ABS");
	if (ioctl(fd, UI_SET_ABSBIT, ABS_X) < 0)
		fprintf(stderr,"error: ioctl UI_SETEVBIT ABS_X");
	if (ioctl(fd, UI_SET_ABSBIT, ABS_Y) < 0)
		fprintf(stderr,"error: ioctl UI_SETEVBIT ABS_Y");
	if (ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE) < 0)
		fprintf(stderr,"error: ioctl UI_SETEVBIT ABS_PRESSURE");

        {
          struct uinput_abs_setup abs_setup;
          struct uinput_setup setup;

          memset(&abs_setup, 0, sizeof(abs_setup));
          abs_setup.code = ABS_X;
          abs_setup.absinfo.value = 0;
          abs_setup.absinfo.minimum = 0;
          abs_setup.absinfo.maximum = 1920;
          abs_setup.absinfo.fuzz = 0;
          abs_setup.absinfo.flat = 0;
          abs_setup.absinfo.resolution = 1;
          if (ioctl(fd, UI_ABS_SETUP, &abs_setup) < 0)
            fprintf(stderr,"error: UI_ABS_SETUP ABS_X");

          memset(&abs_setup, 0, sizeof(abs_setup));
          abs_setup.code = ABS_Y;
          abs_setup.absinfo.value = 0;
          abs_setup.absinfo.minimum = 0;
          abs_setup.absinfo.maximum = 1080;
          abs_setup.absinfo.fuzz = 0;
          abs_setup.absinfo.flat = 0;
          abs_setup.absinfo.resolution = 1;
          if (ioctl(fd, UI_ABS_SETUP, &abs_setup) < 0)
            fprintf(stderr,"error: UI_ABS_SETUP ABS_Y");

          memset(&abs_setup, 0, sizeof(abs_setup));
          abs_setup.code = ABS_PRESSURE;
          abs_setup.absinfo.value = 0;
          abs_setup.absinfo.minimum = 0;
          abs_setup.absinfo.maximum = INT16_MAX;
          abs_setup.absinfo.fuzz = 0;
          abs_setup.absinfo.flat = 0;
          abs_setup.absinfo.resolution = 0;
          if (ioctl(fd, UI_ABS_SETUP, &abs_setup) < 0)
            fprintf(stderr,"error: UI_ABS_SETUP ABS_PRESSURE");

          memset(&setup, 0, sizeof(setup));
          snprintf(setup.name, UINPUT_MAX_NAME_SIZE, "Amogus Touch Screen Device");
          setup.id.bustype = BUS_VIRTUAL;
          setup.id.vendor  = 0x1;
          setup.id.product = 0x1;
          setup.id.version = 2;
          setup.ff_effects_max = 0;
          if (ioctl(fd, UI_DEV_SETUP, &setup) < 0)
            fprintf(stderr,"error: UI_DEV_SETUP");

          if (ioctl(fd, UI_DEV_CREATE) < 0)
            fprintf(stderr,"error: ioctl");
        }
}


void send_event(int type, int code, int value) {
	struct input_event ev;
	ev.type = type;
	ev.code = code;
	ev.value = value;
	if (write(fd, &ev, sizeof(ev)) < 0)
		fprintf(stderr,"error: write()");
}

static int press = 0;
void uinput_event(Event ev){
    printf("Buttons:%d\nX:%d\nY:%d\n\n",
        ev.buttons,
        ev.x,
        ev.y
    );
    
    send_event(EV_ABS, ABS_X, ev.x);
    send_event(EV_ABS, ABS_Y, ev.y);
    send_event(EV_SYN, SYN_REPORT, 1);
    if (ev.buttons != 0 && !press) {
        press = 1;
        send_event(EV_KEY, BTN_LEFT, 1);
        send_event(EV_SYN, SYN_REPORT, 1);
        return;
    }
    if (ev.buttons == 0 && press) {
        press = 0;
        send_event(EV_KEY, BTN_LEFT, 0);
        send_event(EV_SYN, SYN_REPORT, 1);
        return;
    }
}
