#include "mbed.h"

Serial pc(USBTX, USBRX);

DigitalOut pc_activity(LED1);

int speed_left;
int speed_right;

void print_status() {
    printf("\r\nSystem status:\r\n");
    printf("Speed: %d%% %d%%\r\n", speed_left, speed_right);
    printf("Press 'h' for help\r\n");
}

#define SPEED_FACTOR  5
#define MAX_SPEED 100
#define MIN_SPEED -100

bool process_input(unsigned char c) {
    switch(c) {
        case 'q':
            if (speed_left < MAX_SPEED)
                speed_left = speed_left + SPEED_FACTOR;
            break;
        case 'a':
            if (speed_left > MIN_SPEED)
                speed_left = speed_left - SPEED_FACTOR;
            break;
        case 'w':
            if (speed_right < MAX_SPEED)
                speed_right = speed_right + SPEED_FACTOR;
            break;
        case 's':
            if (speed_right > MIN_SPEED)
                speed_right = speed_right - SPEED_FACTOR;
            break;
        case 'h':
            printf("'q/w': increment left/right motor\n\r");
            printf("'a/s': decrement left/right motor\n\r");
            break;
        default:
            return false;
    }
    return true;
}

void init() {
    speed_left = 0;
    speed_right = 0;
}

int main() {
    print_status();
    while(1) {
        if(pc.readable()) {
            unsigned char c = pc.getc();
            if (process_input(c)) {
                pc_activity = !pc_activity;
                print_status();
            }
        }
    }
}
