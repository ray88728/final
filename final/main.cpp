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
Ticker encoder_ticker_left;
Ticker encoder_ticker_right;

PwmOut pin8(D8), pin9(D9);
DigitalIn pin3(D3);
DigitalIn pin4(D4);
DigitalInOut pin13(D13);
#define bound 0.9

DigitalOut led1(LED1);

BBCar car(pin8, pin9, servo_ticker);
FXOS8700CQ acc(PTD9, PTD8, (0x1D<<1));
Serial uart(D12,D11); //tx,rx

volatile int message_num = 0;

volatile int arrivedcount = 0;

volatile bool closed = false;

const char* topic = "Mbed";

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

void sort(float thing[], int length, int max_index, int min_index ){

    float max = thing[0];
    float min = thing[0];
    int index_max =0;
    int index_min =0;
    for( int i=1; i++; i<=length ){
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

int find_thing(float thing[]){

    int max_index, min_index;

    sort(thing, 5, max_index, min_index);

    if((abs(thing[2]-thing[3])<0.5)&&(abs(thing[3]-thing[4])<0.5)&&abs(thing[2]-thing[4])<0.5){
        //RECTANGULAR
        return 0;
    }
    if(max_index == 5){
        //right triangle
        return 0;
    }
    if(2<=max_index && 4>= max_index){
        //triangle
        return 0;
    }
    if(2<=min_index && 4>= min_index){
        //abnormal
        return 1;
    }


}


void messageArrived(MQTT::MessageData& md) {

      MQTT::Message &message = md.message;

      char msg[300];

      sprintf(msg, "Message arrived: QoS%d, retained %d, dup %d, packetID %d\r\n", message.qos, message.retained, message.dup, message.id);

      printf(msg);

      wait_ms(1000);

      char payload[300];

      sprintf(payload, "Payload %.*s\r\n", message.payloadlen, (char*)message.payload);

      printf(payload);

      ++arrivedcount;

}


void publish_message(MQTT::Client<MQTTNetwork, Countdown>* client) {

      MQTT::Message message;
      char buff[100];
      sprintf(buff, "%c",recv);
      message.qos = MQTT::QOS0;
      message.retained = false;
      message.dup = false;
      message.payload = (void*) buff;
      message.payloadlen = strlen(buff) + 1;
      int rc = client->publish(topic, message);

      printf("rc:  %d\r\n", rc);
      printf("Puslish message: %s\r\n", buff);

}


void close_mqtt() {

      closed = true;

}


int main() {
    led1 =1;
    parallax_ping  ping1(pin13);
    parallax_encoder encoder0(pin4, encoder_ticker_left);
    parallax_encoder encoder1(pin3, encoder_ticker_right);
    encoder0.reset();
    encoder1.reset();    
/*    uart.baud(9600);

      wifi = WiFiInterface::get_default_instance();

      if (!wifi) {

            printf("ERROR: No WiFiInterface found.\r\n");

            return -1;

      }



      printf("\nConnecting to %s...\r\n", MBED_CONF_APP_WIFI_SSID);

      int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);

      if (ret != 0) {

            printf("\nConnection error: %d\r\n", ret);

            return -1;

      }



      NetworkInterface* net = wifi;

      MQTTNetwork mqttNetwork(net);

      MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);


      //TODO: revise host to your ip

      const char* host = "192.168.43.151";

      printf("Connecting to TCP network...\r\n");

      int rc = mqttNetwork.connect(host, 1883);

      if (rc != 0) {

            printf("Connection error.");

            return -1;

      }

      printf("Successfully connected!\r\n");


      MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

      data.MQTTVersion = 3;

      data.clientID.cstring = "Mbed";


      if ((rc = client.connect(data)) != 0){

            printf("Fail to connect MQTT\r\n");

      }

      if (client.subscribe(topic, MQTT::QOS0, messageArrived) != 0){

            printf("Fail to subscribe\r\n");

      }
*/
//     car.goStraight(100);
//     while(encoder1.get_cm()<80) wait_ms(50);
//     car.stop();
//     //rotate(90,'r');

//     car.turn(100,-0.1);
//     encoder1.reset();
//     while(encoder1.get_cm()<10.2*2*PI/4) wait_ms(50);
//     car.stop();

//     encoder1.reset();
//     car.goStraight(100);
//     while(encoder1.get_cm()<100) wait_ms(50);
//     car.stop();

//     car.turn(-100,-0.1);
//     encoder1.reset();
//     while(encoder1.get_cm()<10.2*2*PI/4) wait_ms(50);
//     car.stop();

//     encoder1.reset();
//     car.goStraight(-100);
//     while(encoder1.get_cm()<30) wait_ms(50);
//     car.stop();

//     wait_ms(1000);

//     encoder1.reset();
//     car.goStraight(100);
//     while(encoder1.get_cm()<30) wait_ms(50);
//     car.stop();

//     car.turn(100,0.1);
//     encoder0.reset();
//     while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
//     car.stop();

//     encoder1.reset();
//     car.goStraight(100);
//     while(encoder1.get_cm()<70) wait_ms(50);
//     car.stop();

//     car.turn(100,0.1);
//     encoder0.reset();
//     while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
//     car.stop();

//     encoder1.reset();
//     car.goStraight(100);
//     while(encoder1.get_cm()<100) wait_ms(50);
//     car.stop();
    
//    char s[5];

//    sprintf(s,"photo");

//    uart.puts(s);

//    wait(0.5);

//    while(1){

//        if(uart.readable()){

//            recv = uart.getc();


//            printf("%c\r\n",recv);

//            printf("1\r\n");

//            break;
//        }
//    }

//     publish_message(&client);

// ////////// MISSION 2 ///////////////////

//     car.turn(100,0.1);
//     encoder0.reset();
//     while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
//     car.stop();

//     encoder1.reset();
//     car.goStraight(100);
//     while(encoder1.get_cm()<25) wait_ms(50);
//     car.stop();

//     car.turn(100,0.1);
//     encoder0.reset();
//     while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
//     car.stop();

    int find = 0;
    int loop = 0;
    int i;
    while ((find == 0) && (loop < 3)) {
        float thing[5] ={0};
        for (i = 0; i < 5; i++) {
            encoder1.reset();
            encoder0.reset();
            car.goStraight(100);
            while(encoder1.get_cm()<45&&encoder0.get_cm()<45) wait_ms(50);
            car.stop();
            encoder1.reset();
            encoder0.reset();
            car.turn(100,0.1);
            while(encoder1.get_cm()<10.7*2*PI/4&&encoder0.get_cm()<10.7*2*PI/4) wait_ms(50);
            car.stop();
            thing[i] = ping1;
            encoder1.reset();
            encoder0.reset();
            car.back(100,0.1);
            while(encoder1.get_cm()<10.7*2*PI/4&&encoder0.get_cm()<10.7*2*PI/4) wait_ms(50);
            car.stop();

/*            encoder1.reset();
            car.goStraight(100);
            while(encoder1.get_cm()<45) wait_ms(50);
            car.stop();

            wait_ms(1000);

            encoder1.reset();
            car.goStraight(-100);
            while(encoder1.get_cm()<25) wait_ms(50);
            car.stop();

            car.turn(-100,-0.1);
            encoder1.reset();
            while(encoder1.get_cm()<5*2*PI/4) wait_ms(50);
            car.stop();

            encoder1.reset();
            car.goStraight(-100);
            while(encoder1.get_cm()<15) wait_ms(50);
            car.stop();

            car.turn(-100,0.1);
            encoder0.reset();
            while(encoder0.get_cm()<5*2*PI/4) wait_ms(50);
            car.stop();*/
        }
        find = find_thing(thing);
        if(find == 1){
            led1 =0;
        }
    }
    car.turn(-100,0.1);
    encoder0.reset();
    while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
    car.stop();

    encoder1.reset();
    car.goStraight(100);
    while(encoder1.get_cm()<50) wait_ms(50);
    car.stop();

    car.turn(100,0.1);
    encoder0.reset();
    while(encoder0.get_cm()<10.2*2*PI/4) wait_ms(50);
    car.stop();

    encoder1.reset();
    car.goStraight(100);
    while(encoder1.get_cm()<150) wait_ms(50);
    car.stop();

}