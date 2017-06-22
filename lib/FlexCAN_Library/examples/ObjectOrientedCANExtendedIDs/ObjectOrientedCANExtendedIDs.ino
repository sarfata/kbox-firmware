/*
 * Object Oriented CAN example for Teensy 3.6 with Dual CAN buses 
 * By Collin Kidder. Based upon the work of Pawelsky and Teachop
 * 
 * Both buses are set to 500k to show things with a faster bus.
 * The reception of frames in this example is done via callbacks
 * to an object rather than polling. Frames are delivered as they come in.
 */

#include <FlexCAN.h>

#ifndef __MK66FX1M0__
  #error "Teensy 3.6 with dual CAN bus is required to run this example"
#endif

static CAN_message_t msg0,msg1;

elapsedMillis RXtimer;
elapsedMillis LEDtimer;
uint32_t RXCount = 0;
boolean displayCAN = true;
const uint8_t redLEDpin = 2;
boolean redLEDstate;

class CANClass : public CANListener 
{
public:
   void printFrame(CAN_message_t &frame, int mailbox);
   bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

void CANClass::printFrame(CAN_message_t &frame, int mailbox)
{
  if(displayCAN){
   Serial.print(mailbox);
   Serial.print(" ID: ");
   Serial.print(frame.id, HEX);
   Serial.print(" Data: ");
   for (int c = 0; c < frame.len; c++) 
   {
      Serial.print(frame.buf[c], HEX);
      Serial.write(' ');
   }
   Serial.println();
  }
   RXCount++;
 
}

bool CANClass::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
    printFrame(frame, mailbox);

    return true;
}

CANClass CANClass0;
CANClass CANClass1;

// -------------------------------------------------------------
void setup(void)
{
  delay(1000);
  Serial.println(F("Hello Teensy 3.6 dual CAN Test With Objects."));
  pinMode(redLEDpin,OUTPUT);
  
  Can0.begin(1000000);  
  Can1.begin(1000000);
  Can0.attachObj(&CANClass0);
  Can1.attachObj(&CANClass1);
  
  
  CAN_filter_t allPassFilter;
  allPassFilter.id=0;
  allPassFilter.ext=1;
  allPassFilter.rtr=0;

  //leave the first 4 mailboxes to use the default filter. Just change the higher ones
  for (uint8_t filterNum = 4; filterNum < 16;filterNum++){
    Can0.setFilter(allPassFilter,filterNum); 
    Can1.setFilter(allPassFilter,filterNum); 
  }
  for (uint8_t filterNum = 0; filterNum < 16;filterNum++){
     CANClass0.attachMBHandler(filterNum);
     CANClass1.attachMBHandler(filterNum);
  }
  //CANClass0.attachGeneralHandler();
  //CANClass1.attachGeneralHandler();
  
}

// -------------------------------------------------------------
void loop(void)
{
  if (RXtimer > 10000){
    Serial.println("Total Received Messages in 10 Sec:");
    Serial.println(RXCount);
    RXtimer = 0;
    RXCount=0;
    displayCAN = !displayCAN;
  }
  if (LEDtimer >250){
    LEDtimer = 0;
    redLEDstate = !redLEDstate;
    digitalWrite(redLEDpin, redLEDstate);
  }
}
