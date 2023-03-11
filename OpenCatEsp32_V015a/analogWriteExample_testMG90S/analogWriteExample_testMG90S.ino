/*
 Mega analogWrite() test

 This sketch fades LEDs up and down one at a time on digital pins 2 through 13.
 This sketch was written for the Arduino Mega, and will not work on previous boards.

 The circuit:
 * LEDs attached from pins 2 through 13 to ground.

 created 8 Feb 2009
 by Tom Igoe

 This example code is in the public domain.

 */
// These constants won't change.  They're used to give names
// to the pins used:
const int lowestPin = 33;
const int highestPin = 33;
#include <ESP32Servo.h>
Servo myservo;

void setup() {
	Serial.begin(115200);
 pinMode(33, OUTPUT);
	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
 myservo.setPeriodHertz(50); // standard 50 hz servo
   myservo.attach(33, 500, 2200); // Attach the servo after it has been detatched
}

void loop() {
			for (int brightness = 500; brightness <= 2200; brightness++) {
				//analogWrite(33, brightness);
				myservo.write(brightness);
				delay(1);
			}

      for (int brightness = 2200; brightness >= 500; brightness--) {
        //analogWrite(33, brightness);
        myservo.write(brightness);
        delay(10);
      }
}
