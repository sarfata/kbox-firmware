/*
*   It doesn't work for Teensy LC yet!
*/

#include "ADC.h"
#include "RingBufferDMA.h"

const int readPin = A9;

ADC *adc = new ADC(); // adc object


// Define the array that holds the conversions here.
// buffer_size must be a power of two.
// The buffer is stored with the correct alignment in the DMAMEM section
// the +0 in the aligned attribute is necessary b/c of a bug in gcc.
const uint8_t buffer_size = 8;
DMAMEM static volatile int16_t __attribute__((aligned(buffer_size+0))) buffer[buffer_size];

// use dma with ADC0
RingBufferDMA *dmaBuffer = new RingBufferDMA(buffer, buffer_size, ADC_0);

#if ADC_NUM_ADCS>1
//const int buffer_size2 = 8;
//DMAMEM static volatile int16_t __attribute__((aligned(buffer_size2+0))) buffer2[buffer_size2];
//
//// use dma with ADC1
//RingBufferDMA *dmaBuffer2 = new RingBufferDMA(buffer2, buffer_size2, ADC_1);
#endif // defined

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT); //pin 23 single ended

    Serial.begin(9600);

    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REF_EXT.
    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(8); // set number of averages
    adc->setResolution(12); // set bits of resolution


    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

    // enable DMA and interrupts
    adc->enableDMA(ADC_0);

    // ADC interrupt enabled isn't mandatory for DMA to work.
    adc->enableInterrupts(ADC_0);
}

char c=0;


void loop() {

     if (Serial.available()) {
      c = Serial.read();
      if(c=='s') { // start dma
            Serial.println("Start DMA");
            dmaBuffer->start(&dmaBuffer_isr);
      } else if(c=='c') { // start conversion
          Serial.println("Conversion: ");
          adc->analogRead(readPin, ADC_0);
      } else if(c=='p') { // print buffer
          printBuffer();
      } else if(c=='l') { // toggle led
          digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
      } else if(c=='r') { // read
          Serial.print("read(): ");
          Serial.println(dmaBuffer->read());
      } else if(c=='f') { // full?
          Serial.print("isFull(): ");
          Serial.println(dmaBuffer->isFull());
      } else if(c=='e') { // empty?
          Serial.print("isEmpty(): ");
          Serial.println(dmaBuffer->isEmpty());
      }
  }


    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    delay(100);
}

void dmaBuffer_isr() {
    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    Serial.println("dmaBuffer_isr");
    // update the internal buffer positions
    dmaBuffer->dmaChannel->clearInterrupt();
}


// it can be called everytime a new value is converted. The DMA isr is called first
void adc0_isr(void) {
    //int t = micros();
    Serial.println("ADC0_ISR"); //Serial.println(t);
    adc->adc0->readSingle(); // clear interrupt
}


void printBuffer() {
    Serial.println("Buffer: Address, Value");

    uint8_t i = 0;
    // we can get this info from the dmaBuffer object, even though we should have it already
    volatile int16_t* buffer = dmaBuffer->buffer();
    for (i = 0; i < dmaBuffer->size(); i++) {
        Serial.print(uint32_t(&buffer[i]), HEX);
        Serial.print(", ");
        Serial.println(buffer[i]);
    }
//
//    Serial.print("Current pos: ");
//    Serial.println(uint32_t(dmaBuffer->dmaChannel->destinationAddress()), HEX);
//
//    Serial.print("p_elems: ");
//    Serial.println(uint32_t(dmaBuffer->p_elems), HEX);
//
//    Serial.print("b_start: ");
//    Serial.println(dmaBuffer->b_start);
//    Serial.print("b_end: ");
//    Serial.println(dmaBuffer->b_end);

}

