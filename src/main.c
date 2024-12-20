#include "clock.h"
#include "ldo.h"
#include "watchdog.h"
#include "delay.h"
#include "isp.h"
#include "uart.h"
#include "usb.h"
#include "debug.h"
#include "matrix.h"
#include "utils.h"
#include "keyboard.h"
#include "user_init.h"
#include "indicators.h"
#include "uart.h"
#include "kb.h"
#ifdef RF_ENABLED
#    include "rf_controller.h"
#endif

#include "pwm.h" // TODO: interrupt is defined here and need to be imported in main, centralise interupt definitions

#include <stdio.h>

void init()
{
    ldo_init();
    clock_init();
#if DEBUG == 1
    uart_init();
#endif

    user_init();

    matrix_init();
    keyboard_init();
    usb_init();

    // usb interupt priority - 3
    IPH1 |= (1 << 6);
    IPL1 |= (1 << 6);

#if DEBUG == 1
    // uart interupt priority - 2
    IPH1 |= (1 << 6);
    IPL1 |= (0 << 6);
#endif

    EA = 1; // enable interrupts
}

void main()
{
    init();

    delay_ms(1000); // perhaps clocks are unstable at this point? influences UART

    dprintf("SMK v" TOSTRING(SMK_VERSION) "\r\n");
    dprintf("DEVICE vId:" TOSTRING(USB_VID) " pId:" TOSTRING(USB_PID) "\n\r");

    delay_ms(1000);

    kb_init();

#ifdef RF_ENABLED
    rf_init();
#endif

    // enable pwm and interrupt (driving matrix scan)
    indicators_start();

    delay_ms(1000);

    while (1) {
        CLR_WDT();

        kb_update_switches();

        kb_update();

        matrix_task();
    }
}
