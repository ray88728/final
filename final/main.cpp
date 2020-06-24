#include "mbed.h"

#include "arm_math.h"

#include "FXOS8700CQ.h"

#include "bbcar.h"

#include <math.h>

#include <stdlib.h>

Ticker servo_ticker;

Ticker encoder_ticker_left;

Ticker encoder_ticker_right;

PwmOut pin8(D8), pin9(D9);

DigitalIn pin3(D3);

DigitalIn pin4(D4);

#define bound 0.9

BBCar car(pin8, pin9, servo_ticker);

FXOS8700CQ acc(PTD9, PTD8, (0x1D<<1));


float state[3] = {0};

float Kp = 0, Ki = 0, Kd = 0;

float a0 = 0, a1 = 0, a2 = 0;

void pid_init(){

    state[0] = 0;

    state[1] = 0;

    state[2] = 0;

    a0 = Kp + Ki + Kd;

    a1 = (-Kp) - 2*Kd;

    a2 = Kd;

}

float pid_process(float in){

    int out = in*a0 + a1*state[0] + a2*state[1] + state[2];


    //update state

    state[1] = state[0];

    state[0] = in;

    state[2] = out;


    return out;

}


int main() {


    parallax_encoder encoder0(pin3, encoder_ticker_left);

    parallax_encoder encoder1(pin4, encoder_ticker_right);

    encoder0.reset();

    encoder1.reset();    


    car.goStraight(100);

    while(encoder1.get_cm()<100) wait_ms(50);

    car.stop();

    //rotate(90,'r');

    car.turn(100,-0.3);
    encoder1.reset();
    while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);
    //wait(2.2);

    car.stop();

    encoder1.reset();

    car.goStraight(100);

    while(encoder1.get_cm()<200) wait_ms(50);

    car.stop();
    encoder1.reset();
    car.back(100,-0.3);

    while(encoder1.get_cm()<10.7*2*PI/4) wait_ms(50);

    car.stop();

    encoder1.reset();

    car.goStraight(-300);

    wait(2);

    car.stop();

}