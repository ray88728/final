#include "mbed.h"

#include "arm_math.h"

#include "FXOS8700CQ.h"

#include "bbcar.h"

#include <math.h>

#include <stdlib.h>

#include "MQTTNetwork.h"

#include "MQTTmbed.h"

#include "MQTTClient.h"

WiFiInterface *wifi;

Ticker servo_ticker;

Ticker encoder_ticker_left;

Ticker encoder_ticker_right;

PwmOut pin8(D8), pin9(D9);

DigitalIn pin3(D3);

DigitalIn pin4(D4);

#define bound 0.9

BBCar car(pin8, pin9, servo_ticker);

Serial uart(D1,D0); //tx,rx

RawSerial xbee(D12, D11);

void xbee_tx(void);

volatile int message_num = 0;
volatile int arrivedcount = 0;
volatile bool closed = false;

int count =0;

char recv;

const char* topic = "Mbed";

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
      message_num++;
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

    uart.baud(9600);

    parallax_encoder encoder0(pin3, encoder_ticker_left);

    parallax_encoder encoder1(pin4, encoder_ticker_right);

    encoder0.reset();

    encoder1.reset();    

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


    car.goStraight(100);

    while(encoder1.get_cm()<100){

        wait_ms(50);
        count = count + 1;
        if(count == 20){
            xbee_tx('f'); // f means forward
            count =0;
        }
    }

    car.stop();

    encoder1.reset();

    car.turn(100,-0.3);

    while(encoder1.get_cm()<10.7*2*PI/4){

        wait_ms(50);
        count = count + 1;
        if(count == 20){
            xbee_tx('l'); // l means turn left
            count =0;
        }
    }

    car.stop();

    encoder1.reset();

    car.goStraight(100);

    while(encoder1.get_cm()<100){

        wait_ms(50);
        count = count + 1;
        if(count == 20){
            xbee_tx('f'); // f means forward
            count =0;
        }
    }

    car.stop();

    encoder1.reset();

    car.back(100,-0.3);

    while(encoder1.get_cm()<10.7*2*PI/4){

        wait_ms(50);
        count = count + 1;
        if(count == 20){
            xbee_tx('b'); // b means backward
            count =0;
        }
    }

    car.stop();

    encoder1.reset();

    car.goStraight(-100);

    while(encoder1.get_cm()<100){

        wait_ms(50);
        count = count + 1;
        if(count == 20){
            xbee_tx('b'); // b means backward
            count =0;
        }
    }

    car.stop();

    char s[5];

    sprintf(s,"photo");

    uart.puts(s);
    
    wait(0.5);

    while(1){

        if(uart.readable()){

            recv = uart.getc();

            break;
        }
    }

    publish_message(&client);



}

void xbee_tx(char c)

{
    xbee.printf("%c",c);
}