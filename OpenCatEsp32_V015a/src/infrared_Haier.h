/* Copyright 2019 David Conran

  An IR LED circuit *MUST* be connected to the ESP8266 on a pin
  as specified by kIrLed below.

  TL;DR: The IR LED needs to be driven by a transistor for a good result.

  Suggested circuit:
      https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending

  Common mistakes & tips:
*   * Don't just connect the IR LED directly to the pin, it won't
      have enough current to drive the IR LED effectively.
*   * Make sure you have the IR LED polarity correct.
      See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
*   * Typical digital camera/phones can be used to see if the IR LED is flashed.
      Replace the IR LED with a normal LED if you don't have a digital camera
      when debugging.
*   * Avoid using the following pins unless you really know what you are doing:
*     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
*     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
*     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
*   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
      for your first time. e.g. ESP-12 etc.
*/
// #include <Arduino.h>
// #include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Haier.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4
IRHaierAC176 ac(kIrLed);    // Set the GPIO used for sending messages.

struct acHaier              // todo, not needed yet, for future
{
  uint8_t ac_temp;
  uint8_t ac_cool_heat;
  uint8_t ac_fan;
};

void printState() {
  // Display the settings.
  Serial.println("Haier A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());
}

void Haier_AC_setup() {
  ac.begin();
  // Set up what we want to send. See ir_Haier.cpp for all the options.
  Serial.println("Default state of the remote.");
  printState();
  Serial.println("Setting initial state for A/C.");
  ac.setFan(kHaierAcYrw02FanLow);
  ac.setMode(kHaierAcYrw02Heat);
  ac.setTemp(21);
  ac.setSwing(false);
  ac.off();
  printState();
  ac.send();
}

void Haier_AC_handle(int ACcommand) { // receives from io.h

  // Set up what we want to send. See ir_Haier.cpp for all the options.
  //printState();
  switch (ACcommand) {
    case 1:
      ac.off();
      ac.send();
      break;
    case 2:
      ac.on();
      ac.send();
      break;
    case 3:
      ac.setMode(kHaierAcYrw02Heat);
      ac.setFan(kHaierAcYrw02FanLow);
      ac.setTemp(21);
      ac.setSwing(false);
      ac.on();
      ac.send();
      break;
    case 4:
      ac.setMode(kHaierAcYrw02Cool);
      ac.setFan(kHaierAcYrw02FanHigh);
      ac.setTemp(25);
      ac.setSwing(true);
      ac.on();
      ac.send();
      break;
  }

  // exmples:
  // ac.setFan(kHaierAcYrw02FanLow);
  // ac.setFan(kHaierAcYrw02FanHigh);
  // ac.setMode(kHaierAcYrw02Heat);
  // ac.setMode(kHaierAcYrw02Cool);
  // ac.setTemp(21);
  // ac.setSwing(false);
  // ac.setSwing(true);
  // ac.on();
  // ac.off();
  // ac.send();
  // printState();
}
