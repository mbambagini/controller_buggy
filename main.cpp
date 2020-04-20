#include "mbed.h"

#include "xbee.hpp"
#include "ssWiSocket.hpp"

#define SENSOR_PORT_ID          1
#define MOTOR_RIGHT_PORT_ID     3
#define MOTOR_LEFT_PORT_ID      5

#define TX_SPEED_UPLOAD_MS      500
#define FRAME_RATE_MS           800
#define MAIL_LOOP_MS            50

#define SPEED_FACTOR            1
#define MAX_SPEED               100
#define MIN_SPEED               -100

void print_status();
void process_input(unsigned char c);

int speed_left = 0;
int speed_right = 0;

Serial pc(USBTX, USBRX, "pc", 9600);

int main() {
    int counterTx = 0;
    int counterUpdate = 0;
    constexpr int loopsBeforeUpload = int(TX_SPEED_UPLOAD_MS/MAIL_LOOP_MS);
    constexpr int loopsBeforeFrameUpdate = int(FRAME_RATE_MS/MAIL_LOOP_MS);

    // init ssWi protocol
    XBeeModule xbee(p9, p10, 102, 14);
    xbee.setDstAddress(XBeeBroadcastAddress());
    xbee.init(5, 10);
    
    ssWiSocket* leftSocket = ssWiSocket::createSocket(MOTOR_LEFT_PORT_ID);
    ssWiSocket* rightSocket = ssWiSocket::createSocket(MOTOR_RIGHT_PORT_ID);

    print_status();

    while(true) {
        // process user input
        if (pc.readable()) {
            unsigned char c = pc.getc();
            process_input(c);
        }
        // tx speeds
        if (++counterTx == loopsBeforeUpload) {
            counterTx = 0;
            leftSocket->write(speed_left);
            rightSocket->write(speed_right);
        }
        // update frame
        if (++counterUpdate == loopsBeforeFrameUpdate) {
            counterUpdate = 0;
            print_status();
        }

        // main loop end
        thread_sleep_for(MAIL_LOOP_MS);
    }
}

void print_status() {
    static ssWiSocket* sensorSocket = ssWiSocket::createSocket(SENSOR_PORT_ID);
    printf("Distance (%dcm) - Speed (%d%% %d%%)\r\n", sensorSocket->read(), 
                                                      speed_left, speed_right);
}

void print_help() {
    printf("*********************************\n\r");
    printf("'q/w': increment left/right motor\n\r");
    printf("'a/s': decrement left/right motor\n\r");
    printf("*********************************\n\r");
}

void process_input(unsigned char c) {
    switch(c) {
        case 'q':
            speed_left += (speed_left < MAX_SPEED) ? SPEED_FACTOR : 0;
            break;
        case 'a':
            speed_left -= (speed_left > MIN_SPEED) ? SPEED_FACTOR : 0;
            break;
        case 'w':
            speed_right += (speed_right < MAX_SPEED) ? SPEED_FACTOR : 0;
           break;
        case 's':
            speed_right -= (speed_right > MIN_SPEED) ? SPEED_FACTOR : 0;
            break;
        case 'h':
            print_help();
            break;
    }
}
