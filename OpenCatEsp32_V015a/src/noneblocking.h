//// This is myfile.h
//
//#ifndef __MYFILE_H_
//#define __MYFILE_H_
//.
//.
//.
//// your code for header file here
//.
//.
//#endif

#ifndef __NONEBLOCKING_H_
#define __NONEBLOCKING_H_

struct SonicSensor {
  const uint8_t PIN;
  volatile uint32_t triggercount;
  volatile bool activated;
};

SonicSensor trigPin = {TRIGPIN, 0, false};
SonicSensor echoPin = {ECHOPIN, 0, false};

//void IRAM_ATTR isr(void* arg) {attachInterrupt(digitalPinToInterrupt(buttonPin), button_ISR, CHANGE);
//  microSecondStart = micros();
//  SonicSensor* s = static_cast<SonicSensor*>(arg);
//  s->triggercount += 1;
//  s->activated = true;
//}


void IRAM_ATTR set_trig() {
  detachInterrupt(trigPin.PIN);
  microSecondStart = micros();
  //trigPin.triggercount += 1;
  trigPin.activated = true;
}

void IRAM_ATTR set_echo() {
  detachInterrupt(echoPin.PIN);
  microSecondEnd = micros();
  //echoPin.triggercount += 1;
  echoPin.activated = true;
}



void getDistanceMM() {
  detachInterrupt(echoPin.PIN);
  detachInterrupt(trigPin.PIN);
  delayMicroseconds(1);
  pinMode(TRIGPIN, OUTPUT);
  delayMicroseconds(1);
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(5);
  attachInterrupt(trigPin.PIN, set_trig, FALLING);
  delayMicroseconds(5);
  digitalWrite(TRIGPIN, LOW);
  //delayMicroseconds(1);
  pinMode(ECHOPIN, INPUT_PULLUP);
  attachInterrupt(echoPin.PIN, set_echo, FALLING);
}

#endif
