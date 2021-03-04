/*
cable tester, uses buttons on pin 2 and 3 to test and learn, respectively
led chain on pin 4 and 5 to indicate pass/fail condition
attach harness to remaining pins, except 0, must stay empty for reference
this leaves 14 pins on an uno, which will test a 7 wire harness

3/4/2021
Changed to use a mega with buttons on pin 2 and 3, and red/blue led between 4 and 5.
Shorting pin 3 learns the test, shorting pin 2 runs the test.
led's blink to show input/test, and stay lit with good or bad test.

*/


#include "Arduino.h"
#define sw A1
int test=0;
int wait = 0;
int learn = 350;

typedef struct
{
    bool skip;
    byte connectedTo;
}TESTPIN;
const byte LISTLENGTH=60; //change this to the number of pins on the tester

TESTPIN learnlist[LISTLENGTH];
TESTPIN testlist[LISTLENGTH];


const int testPin = 2;
const int learnPin = 3;
const int pin_red = 5;
const int pin_blue = 4;

int testState = 0;
int learnState = 0;


// the setup function runs once when you press reset or power the board
void setup()
{
    pinMode(LED_BUILTIN,OUTPUT);
    Serial.begin(9600);
    //while (!Serial) {
    //    ; // wait for serial port to connect. Needed for native USB port only
    // }
    Serial.println("Cable tester");
    // initialize digital pin 13 as an output.
    pinMode(13, OUTPUT);
    pinMode(testPin, INPUT);
    pinMode(learnPin, INPUT);
    pinMode(pin_red, OUTPUT);
    pinMode(pin_blue,OUTPUT);
    
    learnlist[0].skip=true;//serial port RX0, 0 must always be skipped, as it is the default value of the sampleList items.
    learnlist[1].skip=true;//serial port TX0
    learnlist[2].skip=true; //test button
    learnlist[3].skip=true; //learn button
    learnlist[4].skip=true; //led
    learnlist[5].skip=true; //led
    //learnlist[13].skip=true;
    for(byte i=1;i<LISTLENGTH;i++)
    {
        testlist[i].skip=learnlist[i].skip;
    }
}

// the loop function runs over and over again forever
void loop(){

testState = digitalRead(testPin);
learnState = digitalRead(learnPin);
        
        if (testState == HIGH){//test
            blinkenlight();
            measureList(testlist, LISTLENGTH);
            compareList(learnlist, testlist);
            //printList(testlist);
        }
        else if (learnState == HIGH){//learn
        measureList(learnlist, LISTLENGTH); //learn list
        blinkenlight();
           // printList(learnlist);
        }
           
delay(20);
/*

// Serial input, t to test, l to learn
{
    while (Serial.available())
    {
        // get the new byte:
        switch(Serial.read())
        {
        case 't'://test
            measureList(testlist, LISTLENGTH);
            compareList(learnlist, testlist);
            //printList(testlist);
            break;
        case 'l'://learn
            measureList(learnlist, LISTLENGTH);
           // printList(learnlist);
            break;
        }
    }
}
*/
}

bool compareList(TESTPIN refList[], TESTPIN sampleList[])
{
    char str[40];
    bool ret=true;
    for(byte i=1;i<LISTLENGTH;i++)
    {
        if(refList[i].skip)
        {
            continue;
        }
        if(refList[i].connectedTo != sampleList[i].connectedTo)
        {
            if(i==sampleList[i].connectedTo)
            {
                sprintf(str,"Error: pin %d has no connection to %d", refList[i].connectedTo, i);
            }else
            {
                sprintf(str,"Error: pin %d connected to pin %d", sampleList[i].connectedTo, i);
            }
            Serial.println(str);
            //while (i < 20){
             // digitalWrite(LED_BUILTIN,HIGH);
              //delay(200);
              //digitalWrite(LED_BUILTIN,LOW);
              //delay(200);
              //i++;
            //}
            led_state(pin_red,pin_blue);
            ret=false;
        }
    }
    if(ret)
    {
        Serial.println("All ok");
        //digitalWrite(LED_BUILTIN,HIGH);
        led_state(pin_blue,pin_red);

    }
    return ret;
}

void printList(TESTPIN pinlist[])
{
    char str[40];
    for(byte i=1;i<LISTLENGTH;i++)
    {
        if(pinlist[i].skip)
        {
            continue;
        }
        sprintf(str,"%d : %d", i, pinlist[i].connectedTo);
        Serial.println(str);
    }
}

void measureList(TESTPIN pinlist[], byte length)
{
    //Set all pins to be tested to input pull-up and remove existing connections.
    for(byte i=1;i<length;i++)
    {
        if(pinlist[i].skip)
        {
            continue;
        }
        pinMode(i,INPUT_PULLUP);
        pinlist[i].connectedTo=0;
    }
    //Loop through all the pins and find the connections
    for(byte i=1;i<length;i++)
    {
        if(pinlist[i].skip)
        {
            continue;
        }
        pinMode(i, OUTPUT);
        digitalWrite(i,0);
        for(byte j=i+1;j<length;j++)
        {
            if(pinlist[j].skip || pinlist[j].connectedTo)
            {
                continue;
            }
            if(!digitalRead(j))
            {
                pinlist[j].connectedTo=i;
            }
        }
        if(!pinlist[i].connectedTo)
        {
            pinlist[i].connectedTo=i;
        }
        pinMode(i, INPUT);
    }

}

void led_state(int pin_plus,int pin_ground)
{
  //set both pins low first so you don't have them pushing 5 volts into eachother
  digitalWrite(pin_plus,LOW);  
  digitalWrite(pin_ground,LOW);
  //set output pin as high
  digitalWrite(pin_plus,HIGH);  
  }  

void blinkenlight()
{
        led_state(pin_blue,pin_red);  //visual cue 
        delay(100);
        led_state(pin_red,pin_blue);
        delay(100);
        led_state(pin_blue,pin_red);
        delay(100);
        led_state(pin_red,pin_blue);
        delay(100);
        digitalWrite(pin_red, LOW);
        digitalWrite(pin_blue,LOW);
}
