#include "mbed.h"
#include "arm_math.h"
#include "FXOS8700CQ.h"
#include "bbcar.h"
#include <math.h>
#include <stdlib.h>
#include "MQTTNetwork.h"

#include "MQTTmbed.h"

#include "MQTTClient.h"

char recv;

WiFiInterface *wifi;

Ticker servo_ticker;
//Ticker encoder_ticker_left;
//Ticker encoder_ticker_right;

PwmOut pin8(D8), pin9(D9);
//DigitalIn pin3(D3);
//DigitalIn pin4(D4);
DigitalInOut pin13(D13);
#define bound 0.9

DigitalOut led1(LED1);

BBCar car(pin8, pin9, servo_ticker);
//FXOS8700CQ acc(PTD9, PTD8, (0x1D<<1));
Serial uart(D1,D0); //tx,rx

RawSerial xbee(D12, D11);

volatile int message_num = 0;

volatile int arrivedcount = 0;

volatile bool closed = false;

const char* topic = "Mbed";

int send =0;

void sort(float thing[], int length, int max_index, int min_index ){

    float max = thing[0];
    float min = thing[0];
    int index_max =0;
    int index_min =0;
    for( int i=1; i<length; i++){
        if(max < thing[i]){
            max = thing[i];
            index_max =i;
        }

        if(min > thing[i]){
            min = thing[i];
            index_min =i;
        }
    }
    max_index = index_max;
    min_index = index_min;
}

void find_thing(float thing[],int find){

    int max_index, min_index;

    sort(thing, 5, max_index, min_index);

    if((abs(thing[2]-thing[3])<5)&&(abs(thing[3]-thing[4])<5)&&abs(thing[2]-thing[4])<5){
        //RECTANGULAR
        find= 0;
    }
    else if(max_index == 4){
        //right triangle
        find= 1;
    }
    else if(2<=max_index && 4>= max_index){
        //triangle
        find= 2;
    }
    else {
        //abnormal
        find= 3;
    }


}

EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;

void xbee_rx(void);

char mission;

int main() {
    mission ='1';
    led1 =1;
    parallax_ping  ping1(pin13);
    //parallax_encoder encoder0(pin4, encoder_ticker_left);
    //parallax_encoder encoder1(pin3, encoder_ticker_right);
    //encoder0.reset();
    //encoder1.reset();    
    uart.baud(9600);
    xbee.baud(9600);    
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    queue.call_every(1000,&xbee_rx);
    car.goStraight(100);

//    while(ping1 >=5){wait_us(50);}
     wait(9.7);
     car.stop();
     wait(0.5);
     car.turn(-60,-0.03);
     //encoder1.reset();
     //while(encoder1.get_cm()<10.2*2*PI/4) wait_ms(50);
     wait(1.0);
     car.stop();
     wait(0.8);
     car.turn(-70 ,-0.03);
     //encoder1.reset();
     //while(encoder1.get_cm()<10.2*2*PI/4) wait_ms(50);
     wait(1.0);
     car.stop();
     wait(0.5);

     //encoder1.reset();
     car.goStraight(100);
     //while(encoder1.get_cm()<100) wait_ms(50);
    //while(ping1 >=5){wait_us(50);}

     wait(2.5);
     car.stop();
     wait(0.5);
     car.turn(-60,0.03);
     //encoder1.reset();
     //while(encoder1.get_cm()<10.2*2*PI/4) wait_ms(50);
     wait(2);
     car.stop();
     car.goStraight(-100);
     wait(0.5);
     car.stop();
    char s[5];

    sprintf(s,"photo");

    uart.puts(s);

    wait(0.5);

    while(1){

        if(uart.readable()){

            recv = uart.getc();


            printf("%c\r\n",recv);

            printf("1\r\n");

            break;
        }
    }
     send =1;
     xbee.printf("o");
     wait(0.1);
     xbee.printf("%c",recv);
     send =0;
     car.turn(100,0.03);
     //encoder1.reset();
     //while(encoder1.get_cm()<10.2*2*PI/4) wait_ms(50);
     wait(1.5);
     car.stop();
     wait(0.5);



     //encoder1.reset();
     car.goStraight(100);
     //while(encoder1.get_cm()<30) wait_ms(50);
     wait(4.8);
     car.stop();
     wait_ms(1000);
     car.turn(60,-0.03);
     //encoder1.reset();
     //while(encoder1.get_cm()<10.2*2*PI/4) wait_ms(50);
     wait(1.5);
     car.stop();
     wait(0.5);

     car.turn(60,-0.03);
     //encoder1.reset();
     //while(encoder1.get_cm()<10.2*2*PI/4) wait_ms(50);
     wait(0.5);
     car.stop();
     wait(0.5);


    car.goStraight(-100);
    wait(2.5);

    car.stop();
    wait(1);
    // encoder1.reset();
     car.goStraight(100);
     //while(encoder1.get_cm()<30) wait_ms(50);
     wait(2.5);
     car.stop();
     wait(0.5);
     car.turn(-100,0.03);
     wait(1.4);
     //encoder0.reset();
     //while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
     car.stop();
     wait(0.5);
    // encoder1.reset();
     car.goStraight(100);
     //while(encoder1.get_cm()<70) wait_ms(50);
     wait(4.5);
    //while(ping1 >=50){wait_us(50);}
     car.stop();
     wait(0.5);
     car.turn(-90,0.03);
     wait(0.8);
     car.stop();
     wait(0.4);
     car.turn(-90,0.03);
     wait(0.8);

     //encoder0.reset();
     //while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
     car.stop();
     wait(0.5);
    // encoder1.reset();
     car.goStraight(100);
     //while(encoder1.get_cm()<100) wait_ms(50);
    //while(ping1 >=5){wait_us(50);}
     wait(7.8);
     car.stop();
//     publish_message(&client);

     led1 =0;

     wait(1);

     led1 =1;

// ////////// MISSION 2 ///////////////////

     car.turn(-90,0.03);
     wait(1.6);
     car.stop();
     wait(0.4);

//     encoder1.reset();
//     car.goStraight(100);
//     while(encoder1.get_cm()<25) wait_ms(50);
//     car.stop();

//     car.turn(100,0.1);
//     encoder0.reset();
//     while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
//     car.stop();
    mission ='2';
    int find=10;
    int i;
    float thing[5]={0};
    float max, min;
    int max_index, min_index;
    
    car.goStraight(100);
    wait(1);
    car.stop();
    wait(0.5);

     car.turn(-100,0.03);
     wait(1.6);
     car.stop();
     wait(0.4);

    car.goStraight(100);
    wait(1);
    car.stop();
    wait(0.5);

     car.turn(-90,-0.03);
     wait(0.9);
     car.stop();
     wait(0.4);
     car.turn(-90,-0.03);
     wait(0.5);
     car.stop();
     wait(0.5);


    car.goStraight(100);
    wait(1.4);
    car.stop();
    wait(0.5);

    car.turn(-100,0.03);
    
    for(i=0; i<=2; i++){
        wait(0.2);
        thing[i]=ping1;        
    }

    car.stop();
    wait(0.5);
    
    car.turn(100,0.03);
    
    for(i=0; i<=2; i++){
        wait(0.2);
        //thing[i]=ping1;        
    }
    car.stop();
    wait(0.5);
    
    car.turn(-100,-0.03);
    
    for(i=3; i<=4; i++){
        wait(0.2);
        thing[i]=ping1;        
    }

    car.stop();
    wait(0.5);

    car.turn(100,-0.03);
    
    for(i=3; i<=4; i++){
        wait(0.2);
    }

    car.stop();
    wait(0.5);
    send =1;
    find_thing(thing,find);
    xbee.printf("m");
    wait(0.1);
    xbee.printf("%d",find);
//    car.turn(100,0.03);
    send =0;
    car.goStraight(-100);
    wait(1.0);

    car.turn(100,-0.03);
    wait(1.5);
    car.stop();
    wait(0.5);
    //if(find ==0)
    //    led1=0;
    //printf("No\r\n");

    car.goStraight(-100);
    wait(1);
    car.stop();
    wait(0.5);

    car.turn(100,0.03);
    wait(1.6);
    car.stop();
    wait(0.5);

//    car.turn(100,-0.3);
//    wait(2);
//    car.stop();
    car.goStraight(200);
    wait(5);

    car.turn(-100,0.03);
    wait(1.5);
    car.stop();
    wait(0.5);

    car.goStraight(100);
    wait(8);
    car.stop();

    xbee.printf("f");
    led1 =1;
    car.stop();
}


void xbee_rx(void)
{
    if(send ==0)
        xbee.printf("%c",mission);
    else
    {
        ;
    }
    
  //printf("mission=%d\r\n",mission);
}