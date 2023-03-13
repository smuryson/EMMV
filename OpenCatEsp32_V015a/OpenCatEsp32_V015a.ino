//add WiFi and OTA with: ArduinoOTA.setPassword("adminCAT");  //very trustworthy password :-) 
//password helps to prevent accidently overwriting the wrong target
//void i2c_eeprom_write_byte() reads first and if different, content gets written

#include <WiFi.h>
//#include <WiFiClient.h> //CI for webserver
//#include <WebServer.h> //CI for webserver
//#include <WiFiClientSecure.h> //CI for webserver
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include "Arduino.h" 
#include "soc/soc.h"            // to disable brownout detector -> black out = total loss of Voltage
#include "soc/rtc_cntl_reg.h"   // to disable brownout detector -> brown out = dip in Voltage supply
#include "driver/rtc_io.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#define QUICKandROUGH_TEST 1    // just to test the simple ESP32 board without any peripherals
char* ssid = "";                // Insert your credentials
char* password = "";
// Set your Static IP address
IPAddress local_IP(192, 168, 0, 92);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

#define APSSID "NybbleMK_CAT"
#define APPSK "thereisnospoon"  // standard password :-)
int apConnectingCounterSet = 10;
int apConnectingCounter = apConnectingCounterSet;
bool switchToAP = false;

uint32_t getXtalFrequencyMhz(); // In MHz
uint32_t Freq = 0;
esp_chip_info_t chip_info;


//WHICH ROBOT ********************************************************************************
//#define BITTLE                //Petoi 9 DOF robot dog: 1 on head + 8 on leg
#define NYBBLE                  //Petoi 11 DOF robot cat: 2 on head + 1 on tail + 8 on leg
//#define CUB
//********************************************************************************************

/*RESISTORS 
-> ESP32 can only take 3.3V Volt, so we need to adjust the ~8V Input accordingly!

GND -> connect to -> 2K res (Rb) -> solder to -> 5K1 res (Ra) -> connect to -> VIN

|GND| - |Rb 2K| x |Ra 5K1| - |VIN| - |Battery +| 
          |VOUT GPIO 36|

-> VOUT pin 36 is input only, sensor VP
 
https://byjus.com/voltage-divider-calculator/
Voltage Divider Formula: Vout = (Rb/(Ra+Rb))* Vin

  Ra 5K1 ohm = 5100
  Rb 2K ohm = 2000

  VIN = 6.6V -> Vout ~1.85915 Volts

  VIN = 7.8V -> Vout ~2.14084 Volts

  -> also in reaction.h
*/

//WHICH BOARD *******************************************************************************************************************
#define BiBoard               //ESP32 Board with 12 channels of built-in PWM for joints
                              //BiBoard VOLTAGE pin 36 (VP, input only, via voltage devider) 4.2V-3.3V range
                              //for 18650, two in series (8.4 to 6.6V: ~R1 4700/ R2 2200 OHM gives:) >>> ~2.7-2.1 V

//ADDED O/C (=our case)
#define BiBoard_i2cPWM        //ESP32 adding 16 channels i2c pwm (PCA9685),  all other settings can stay the same as for the BiBoard
//                              BiBoard VOLTAGE pin 36 (VP, input only, via voltage devider), Li 18650 voltage range between 4.2 V-3.3 V,
//                              two in series: ~Ra 5100/ Rb 2000 OHM>>> ~2.2-1.8V
//                              OE PIN on PWM board: OE = Output enable -> can be used to quickly disable all outputs; LOW per default (optional pin)
//                                  OE is LOW: all pins enabled
//                                  OE is HIGH: all pins disabled                 
//                              SHOULD BE ENABLED FOR SAFETY -> so NOT optional for us!

//#define BiBoard2            //ESP32 Board with 16 channels of PCA9685 PWM for joints, //#define VOLTAGE 4 in BiBoard2
//********************************************************************************************************************************

//Send a 'R' token from the serial terminal to reset the birthmark in the EEPROM so that the robot will restart to reset
//#define AUTO_INIT           //activate it to automatically reset joint and imu calibration without prompts

//you can also activate the following modes (they will disable the gyro to save programming space)
//allowed combinations: RANDOM_MIND + ULTRASONIC, RANDOM_MIND, ULTRASONIC, VOICE, CAMERA
#ifndef QUICKandROUGH_TEST
#define ULTRASONIC          //for Nybble's ultrasonic sensor
#endif
//#define VOICE             //for LD3320 module
//#define CAMERA            //for Mu Vision camera
//#define RANDOM_MIND

//#define ALLROTATELIMIT 90     // range, if defined, servos can be checked and rotated between 90 -/+ (ALLROTATELIMIT/2) in setup().
//                                 The program is not moving on automatically.

#define BLT_FIXED_SUFFIX "MK" //if defined: random suffix won't be added to Nyyble Bluetooth name in I2cEEPROM.h when setting up a new board

#define LOW_VOLTAGE_LED 2 // blue LED -> turns on when power low

#include "src/OpenCat.h"

//-------------------------------------------webserver, camera might need more elabarote server //CI for webserver START
/*WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "hello from WebCat!");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}*/
//==========================================================webserver //CI for webserver END

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);    // to disable brownout detector
  digitalWrite(DISABLE_PWM, HIGH);              // disables the pwm board
  pinMode(DISABLE_PWM, OUTPUT);

  digitalWrite(TXD2, LOW);                      //to keep pin 12 at the camera low at boot
  pinMode(TXD2, OUTPUT);                        // the idea is to set a defined output first and then telling switching to output mode
  delay(3000);                                  // give the camera time to boot (as pin 12 is being used)

  Serial.begin(115200);                         // common serial
  Serial.setTimeout(5);                         // todo, what is the best timing?
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);// to communicate with the camera module
  Serial.println("setup");
  PTLF("Flush the serial buffer...");
  while (Serial.available() && Serial.read()) delay(1);    // empty buffer
  while (Serial2.available() && Serial2.read()) delay(1);  // empty buffer
  PTLF("\n* Start *");
  //------------------------------------------------------AC HAIER just for testing here
  Haier_AC_setup();                                        // currently initializing and switch off
  //======================================================AC HAIER
#ifdef EXTREME_LOW_VOLTAGE                      //test if external power supply is there -> usb only not enough power
  test_external_power();                        //disables PWM board if power is too low
#endif

//notify what robot is in use
#ifdef BITTLE
  PTLF("Bittle");
#elif defined NYBBLE
  PTLF("Nybble");
#elif defined CUB
  PTLF("Cub");
#endif

#ifndef QUICKandROUGH_TEST
  i2cDetect();                                  //x, x means critical, should be used in final program
  i2cEepromSetup();                             //x
  //  Serial.println("i2cEepromSetup done");
  //  delay(400);
#endif

#ifndef QUICKandROUGH_TEST  //PAPA HAS POSSIBLY COMMENTED OUT?
  bleSetup();                                   //x moved up from below
  blueSspSetup();                               //x moved up from below
#endif

#ifdef GYRO_PIN
#ifndef QUICKandROUGH_TEST
  //  Serial.println("gyro_pin");               //just for showing where the program is
  //  delay(400);
  imuSetup();                                   //x
  //  Serial.println("imu done");
  //  delay(400);
#endif
#endif
  servoSetup();

  skillList = new SkillList();
  for (byte i = 0; i < randomMindListLength; i++) {
    randomBase += choiceWeight[i];
  }

#ifdef NEOPIXEL_PIN
  ledSetup();
#endif

#ifdef PWM_LED_PIN
  pinMode(PWM_LED_PIN, OUTPUT);
#endif

QA();                                         // quality assurance module

i2c_eeprom_write_byte(EEPROM_BIRTHMARK_ADDRESS, BIRTHMARK);  //finish the test and mark the board as initialized

#ifdef VOLTAGE                                  // todo, moved down; in general watching the voltage of the power supply
  digitalWrite(LOW_VOLTAGE_LED, LOW); // Blue LED off, goes on in reaction.h when low voltage
  pinMode(LOW_VOLTAGE_LED, OUTPUT);
  //while (lowBattery());
  lowBattery();
  Serial.println("low battery done");
#endif

#ifdef VOICE
  voiceSetup();
#endif

#ifdef CAMERA                                   // that is the original, but different camera approach
  cameraSetup();
#endif

  lastCmd[0] = '\0';
  newCmd[0] = '\0';

  //  if (exceptions) {// Make the robot enter joint calibration state (different from initialization) if it is upside down.
  //    strcpy(newCmd, "calib");
  //    exceptions = 0;
  //  }
  //  else {// Otherwise start up normally
  //    strcpy(newCmd, "rest");
  //    token = 'd';
  //    newCmdIdx = 6;
  //  }
  //  loadBySkillName(newCmd);

  playMelody(melodyNormalBoot, sizeof(melodyNormalBoot) / 2);
#ifdef GYRO_PIN
  read_IMU();                                   //ypr is slow when starting up. leave enough time between IMU initialization and this reading
  token = (exceptions) ? T_CALIBRATE : T_REST;  //put the robot's side on the table to enter calibration posture for attaching legs
  newCmdIdx = 2;
#endif

  PTLF("k");
  PTL("Ready!");
  idleTimer = millis();
  beep(24, 50);

#ifdef ALLROTATELIMIT
  allRotate();
#endif

  //---------------------------------------------------- WiFi, OTA, System
  WiFi.config(local_IP, gateway, subnet);
  //  Serial.print("\nsetup() is running on core ");
  //  Serial.println(xPortGetCoreID());
  //  Serial.setDebugOutput(true);
  Serial.println();

  WiFi.begin(ssid, password);
  // WiFi.setSleep(false);                      // Not to use when wifi and bluetooth!!!
  Serial.println("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {       // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print(WL_CONNECTED);                 // just the status#, was '.'
    apConnectingCounter -= 1;                   // Counting down if no shared WiFi is available. Then switching to access point.
    if (apConnectingCounter <= 0) {
      apConnectingCounter = apConnectingCounterSet;
      switchToAP = true;
      break;
    }
  }
  //  Maintain WiFi connection
  if (!switchToAP) {                            // shared WiFi is available
    Serial.println('\n');
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());                // connected to network name
    // Serial.print("IP address:\t");
    // Serial.println(WiFi.localIP());          // ESP32 IP-address
  } else {
    // No shared WiFi is available. Switching to access point.
    WiFi.mode(WIFI_AP);
    Serial.println('\n');
    Serial.println("Configuring access point...");
    char* ssid = APSSID;
    char* password = APPSK;
    // Serial.println(ssid);
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ssid, password);
  }

  Serial.print("Web Cat Ready! Use 'http://");
  if (!switchToAP) {
    Serial.print(WiFi.localIP());
    Serial.println("' to connect.");
  } else {
    IPAddress myIP = WiFi.softAPIP();
    Serial.print(myIP);
    Serial.println("' to connect.");
    Serial.print("AP SSID: ");
    Serial.println(APSSID);
  }
  //  if (!MDNS.begin(APSSID)) {                // using ota name later on
  //    Serial.println("Error setting up MDNS responder!");
  //    while (1) {
  //      delay(1000);
  //    }
  //  }
  //Serial.println("mDNS responder started");

  ArduinoOTA.setHostname("otaCat");
  ArduinoOTA.setPassword("adminCAT");           //  It is a good measure to set the OTA password related to the program.
  //                                                If the Network changes, it will help to prevent accidently overwriting the wrong target.
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else  // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("OTA begin");
  //==================================================== WiFi, OTA, System

  //---------------------------------------------------- chip_info
  //Serial.println(getXtalFrequencyMhz());      // In MHz
  //setCpuFrequencyMhz(10);
  esp_chip_info(&chip_info);
  printf("ESP32 chip with %d CPU cores, WiFi%s%s, ", chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "", (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
  printf("chip revision %d, ", chip_info.revision);
  printf("%dMB %s flashmemory\n", spi_flash_get_chip_size() / (1024 * 1024), (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
  Freq = getCpuFrequencyMhz();
  Serial.print("CPU Freq = ");
  Serial.print(Freq);
  Serial.println(" MHz");
  Freq = getXtalFrequencyMhz();
  Serial.print("XTAL Freq = ");
  Serial.print(Freq);
  Serial.println(" MHz");
  Freq = getApbFrequency();
  Serial.print("APB Freq = ");
  Serial.print(Freq);
  Serial.println(" Hz");
  //==================================================== chip_info

  //---------------------------------------------------- webserver //CI for webserver START
  /*server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "WebCat: this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");*/
  //==================================================== webserver //CI for webserver END

  #ifdef IR_PIN //MOVED FROM AFTER '#ifdef PWM_LED_PIN'
    irrecv.enableIRIn();
  #endif
}

void loop() {
  ArduinoOTA.handle();
#ifdef VOLTAGE
  lowBattery();                                 //x
#endif

  //---------------------------------------------------- webserver //CI for webserver START
  //server.handleClient();
  //==================================================== webserver //CI for webserver END

  //—self-initiative
  //  if (autoSwitch) { //the switch can be toggled on/off by the 'z' token
  //randomMind();//allow the robot to auto rest and do random stuff in randomMind.h //x
  //    powerSaver(POWER_SAVER);//make the robot rest after a certain period, the unit is seconds
  //
  //  }
  //— read environment sensors (low level)
  readEnvironment();                            //x
  readSignal();                                 //x
  //  readHuman();

  //— special behaviors based on sensor events
  dealWithExceptions();  //low battery, fall over, lifted, etc. //x

  //— generate behavior by fusing all sensors and instruction
  //  decision();

  //— action
  //playSound();
#ifdef NEOPIXEL_PIN
  playLight();
#endif
  reaction();                                   //x
  //delay(10);
  //Serial.println(" ota");
}
