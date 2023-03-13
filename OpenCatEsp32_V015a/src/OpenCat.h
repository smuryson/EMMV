/* BiBoard by 李荣仲 (Li Rongzhong)
    PWM:
                        |--------------------------------
                        |    PWM[0]           PWM[6]    |
                        |    PWM[1]           PWM[7]    |
                        |    PWM[2]           PWM[8]    |
                        |-----------                    |
                        |           |                   |
                        |   ESP32   |  IMU         USB-C|~~~~Tail~~~~
                        |           |                   |
                        |-----------                    |
                        |    PWM[3]           PWM[9]    |
                        |    PWM[4]           PWM[10]   |
                        |    PWM[5]           PWM[11]   |
                        |-------------------------------|

    Pin Name    |   ESP32 Pin   |   Arduino Pin Name    |   Alternative Function
    PWM[0]          GPIO4               4                   GPIO / Ain / Touch
    PWM[1]          GPIO5               5                   GPIO / VSPI SS
    PWM[2]          GPIO18              18                  GPIO / VSPI SCK
    -----------------------------------------------------------------------------
    PWM[3]          GPIO32              32                  GPIO / Ain / Touch
    PWM[4]          GPIO33              33                  GPIO / Ain / Touch
    PWM[5]          GPIO19              19                  GPIO / VSPI MISO
    -----------------------------------------------------------------------------
    PWM[6]          GPIO2               2                   boot pin, DO NOT PUT HIGH WHEN BOOT!
    PWM[7]          GPIO15              15                  GPIO / HSPI SS / Ain Touch
    PWM[8]          GPIO13              13                  built-in LED / GPIO / HSPI MOSI / Ain / Touch
    -----------------------------------------------------------------------------
    PWM[9]          GPIO12              12                  GPIO / HSPI MISO / Ain / Touch
    PWM[10]         GPIO14              14                  GPIO / HSPI SCK / Ain / Touch
    PWM[11]         GPIO27              27                  GPIO / Ain / Touch

    I2C:

    Pin Name    |   ESP32 Pin   |   Arduino Pin Name    |   Alternative Function
    I2C-SCL         GPIO22              22                  Fixed - ICM20600 - Pulled
    I2C-SDA         GPIO21              21                  Fixed = ICM20600 - Pulled

    System default, nothing to declaration!

    Other Peripherals:

    Pin Name    |   ESP32 Pin   |   Arduino Pin Name    |   Alternative Function
    IR_Remote       GPIO23              23                  Fixed - VS1838B IR
    DAC_Out         GPIO25              25                  Fixed - PAM8302
    IMU_Int         GPIO26              26                  Fixed - MPU6050 Interrupt

	TRIGPIN 							17 // adjust as needed: Digital pin connected to the trig pin of the ultrasonic sensor
	ECHOPIN 							17 // pin 17 is TX2

    System default, nothing to declaration!
*/

/* BiBoard2, changed later
  IMU_Int     27
  BUZZER      14
  VOLTAGE     4
  RGB LED     15
  GREEN-LED   5
*/

/*  Total DOF            Walking DOF
                   Nybble    Bittle    Cub
   BiBoard  (12)  skip 0~4  skip 0~4    12
   BiBoard2 (16)  skip 0~8  skip 0~8  skip0~4
*/

//birthmark to check whether board has already been initialised
#define BIRTHMARK 'x' //Send 'R' token to reset the birthmark in the EEPROM -> robot will restart/reset
//const uint16_t kIrLed = 4;  // initialized in infraredHaier.h ESP8266 GPIO pin to use. Recommended: 4 

//NOT OUR BOARD /////////////////////////////////////////
#if defined BiBoard && not defined BiBoard_i2cPWM
#define GYRO_PIN
#define PWM_NUM 12 //how many servos in use
#define INTERRUPT_PIN 26  // use pin 2 on Arduino Uno & most boards
#define BUZZER 25
#define VOLTAGE 36 // VPinput only; calculation in reaction.h todo include resisor divider?
#define LOW_VOLTAGE 6.6 
#define IR_PIN 14  

//PWM_NUM -> 12 //MOVED FROM BEFORE #define NYBBLE || BITTLE
//                                headPan, tilt, tailPan, NA
//L:Left-R:Right-F:Front-B:Back---LF, RF, RB, LB
const uint8_t PWM_pin[PWM_NUM] = {19,  4,  2, -1,   //head or shoulder roll {19,  4,  2, 27,
                                  33,  5, 15, 14,   //shoulder pitch or hips
                                  32, 18, 13, 12    //knee
                                 };

//PWM BOARD SERVO POSITIONING
//m move or c calibrate servos:
// m0 head pan
// m1 head tilt
// m2 tail pan
// m3 --
// m4 --
// m5 --
// m6 --
// m7 --
// m8 is shoulder left front
// m9 is shoulder right front
// m10 is shoulder right back
// m11 is shoulder left back
// m12 knee left front
// m13 knee right front
// m14 knee right back
// m15 knee left back
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33

#if defined NYBBLE || defined BITTLE //#define SERVO_FREQ 240 todo changed to 50
#define SERVO_FREQ 50
#else //CUB
#define SERVO_FREQ 240
#endif

//THE BOARD WE ARE USING////////////////////////////////////////
#else //BiBoard2
#define RXD2 35 //input only
#define TXD2 32 // below was for touch
#define GYRO_PIN
#define PWM_NUM 16 //how many servos in use
#define INTERRUPT_PIN 26  // todo originally it was 27  |use pin 2 on Arduino Uno & most boards
#define BUZZER 25 // was 14
#define VOLTAGE 36 // VPinput only, was 4; , calculation in reaction.h
#define LOW_VOLTAGE 6.6 //blue LED turns on
#define EXTREME_LOW_VOLTAGE 5.5 // if defined, then tested in setup() and disables the PWM board; also in reaction.h tested if VOLTAGE defined

#define vFactor 305 //= ~2400/7.8

//#define vFactor 381.82  //: 2520 / 6.6 //PAPA

#define DISABLE_PWM 27 // pin used to disable PWM board and servos
bool pwmDisabled = 1;
//#define NEOPIXEL_PIN 15
//#define PWM_LED_PIN  5
#define IR_PIN 14 // pin IR receiver, was 23 for some reason

// #define TOUCH0 12
// #define TOUCH1 13
// #define TOUCH2 32 used for TXD2
// #define TOUCH3 33

//PWM_NUM -> 16
//                                headPan,  tilt,   tailPan,  NA
const uint8_t PWM_pin[PWM_NUM] = {  0,        1,      2,      -1,     //head+tail
                                  - 1,       -1,      -1,     -1,     //shoulder roll
                                    8,        9,      10,     11,     //shoulder pitch
                                  //13,       10,      6,      2,     //shoulder roll
                                  //14,        9,      5,      1,     //shoulder pitch
                                    12,       13,      14,     15     //knee
                                 };
//L:Left R:Right F:Front B:Back     LF,       RF,      RB,     LB
/*
  0		    1		  2		 3	 4	 5	 6	 7	 8	 9	 10	 11	 12	 13	 14	 15
  HPan	HTilt	TPan	-1	-1	-1	-1	-1	SLF	SRF	SRB	SLB	KLF	KRF	KRB	KLB
  0,		  7,	 -3,	 1,	 3,	 0,	 0,	 0,	-7, -7,	 -3, -5, 0,	 1,	 2,	 -4,
*/

//#define SERVO_FREQ 240 todo changed to 50 //also done ealier, cann be commented out?
#define SERVO_FREQ 50
#endif

//disables PWM board if power too low
int test_external_power() {
  //todo use names than numbers
  float voltage = analogRead(VOLTAGE);
  //if incoming V is < 5.5V disables PWM board
  if ((voltage / vFactor) < EXTREME_LOW_VOLTAGE) {
    pwmDisabled = 1;
    digitalWrite(DISABLE_PWM, HIGH);
    Serial.print(voltage / vFactor); Serial.print("V: ");
    Serial.println(" >>> PWM BOARD DISABLED!");
  }
  else {
    pwmDisabled = 0;
  }
  if (!pwmDisabled) digitalWrite(DISABLE_PWM, LOW); //enable PWM board -> OE is LOW: all pins enabled
  return voltage;
}

#define DOF 16
#if defined NYBBLE || defined BITTLE
#define WALKING_DOF 8
#define GAIT_ARRAY_DOF 8
#else //CUB
#define WALKING_DOF 12
#define GAIT_ARRAY_DOF 8
#endif

enum ServoModel_t {
  MG90S = 0,
  G41,
  P1S,
  P2K
};

//Tutorial: https://bittle.petoi.com/11-tutorial-on-creating-new-skills
#ifdef NYBBLE
#define HEAD
#define TAIL
#define X_LEG
#define REGULAR MG90S //G41
#define KNEE MG90S //G41
#include "InstinctNybbleESP.h" //all Nybble skills

#elif defined BITTLE
#define HEAD
#define LL_LEG
#define REGULAR P1S
#define KNEE P1S
#include "InstinctBittleESP.h" //all Bittle skills

#elif defined CUB
#ifdef BiBoard2
#define HEAD
#define TAIL
#endif
#define LL_LEG
#define REGULAR P1S
#define KNEE P2K
#include "InstinctCubESP.h" //all cub skills
//#define MPU_YAW180
#endif

//李荣仲 uses different servos for knees; we do not
ServoModel_t servoModelList[] = {
  REGULAR, REGULAR, REGULAR, REGULAR,
  REGULAR, REGULAR, REGULAR, REGULAR,
  REGULAR, REGULAR, REGULAR, REGULAR,
  KNEE, KNEE, KNEE, KNEE
};

bool newBoard = false;

#include <math.h>
//token list
#define T_ABORT       'a'
#define T_BEEP        'b'
#define T_BEEP_BIN    'B'
#define T_CALIBRATE   'c'
#define T_COLOR       'C'
#define T_REST        'd'
#define T_GYRO        'g'
#define T_HELP        'h'
#define T_INDEXED_SIMULTANEOUS_ASC  'i'
#define T_INDEXED_SIMULTANEOUS_BIN  'I'
#define T_JOINTS      'j'
#define T_SKILL       'k' //needs to be added before every skill
#define T_SKILL_DATA  'K'
#define T_LISTED_BIN  'L'
#define T_MOVE_ASC    'm'
#define T_MOVE_BIN    'M'
#define T_MELODY      'o'
#define T_PAUSE       'p'
#define T_POWER       'P'
#define T_RAMP        'r'
#define T_RESET       'R'
#define T_SAVE        's'
#define T_SERVO_MICROSECOND 'S'
#define T_TILT        't'
#define T_TEMP        'T'           //call the last skill data received from the serial port
#define T_MEOW        'u'
#define T_PRINT_GYRO            'v' //print Gyro data
#define T_VERBOSELY_PRINT_GYRO  'V' //verbosely print Gyro data
#define T_WORD        'w'
#define T_XLEG        'x'
#define T_RANDOM_MIND 'z'
//#define T_WRITE_SKILL 'Z'         //todo new shortcut to write skills
#define T_SEND_IR_AC 'A'            //todo new shortcut to send infrared signal, referring to AC
#define T_SEND_IR_W 'W'             //todo new shortcut to send infrared signal, referring to 'W'hatever generic device
#define T_ACCELERATE  '.'
#define T_DECELERATE  ','


bool updated[10];
float degPerRad = 180 / M_PI;
float radPerDeg = M_PI / 180;

//control related variables
#define CHECK_BATTERY_PERIOD 5000 //todo was 10000, now every 5 seconds
int uptime = -1;
int frame;
int tStep = 1;
char token;
char lastToken;
#define CMD_LEN 10  //the last char will be '\0' so only CMD_LEN-1 elements are allowed 
char *newCmd = new char[CMD_LEN];
char *lastCmd = new char[CMD_LEN];
int cmdLen;
byte newCmdIdx = 0;
int8_t* dataBuffer = new int8_t[1524];
int lastVoltage;

bool checkGyro = false;
bool printGyro = false;
bool autoSwitch = false;
bool walkingQ = false;
bool imuUpdated;
byte exceptions = 0;
byte transformSpeed = 2;
float protectiveShift;//reduce the wearing of the potentiometer


bool initialBoot = true;
bool safeRest = true;

int delayLong = 14;
int delayMid = 8;
int delayShort = 2;
int delayStep = 1;
int runDelay = delayMid;

#ifdef NYBBLE
int8_t middleShift[] = {0, 15, 0, 0,
                        -45, -45, -45, -45,
                        10, 10, -10, -10,
                        -30, -30, 30, 30
                       };
#elif defined BITTLE
int8_t middleShift[] = {0, 15, 0, 0,
                        -45, -45, -45, -45,
                        55, 55, -55, -55,
                        -55, -55, -55, -55
                       };

#else //CUB
int8_t middleShift[] = {0, 15, 0, 0,
                        -45, -45, -45, -45,
                        55, 55, -55, -55,
                        -45, -45, -45, -45
                       };
#endif

#ifdef CUB
int8_t rotationDirection[] = {1, -1, 1, 1,
                              1, -1, 1, -1,
                              1, -1, -1, 1,
                              1, -1, -1, 1
                             };
int angleLimit[][2] = {
  { -120, 120}, { -30, 80}, { -120, 120}, { -120, 120},
  { -90, 60}, { -90, 60}, { -90, 90}, { -90, 90},
  { -180, 120}, { -180, 120}, { -80, 200}, { -80, 200},
  { -66, 100}, { -66, 100}, { -66, 100}, { -66, 100},
};
#else //NYBBLE + BITTLE
int8_t rotationDirection[] = {1, -1, 1, 1,
                              1, -1, 1, -1,
                              1, -1, -1, 1,
                              -1, 1, 1, -1
                             };
int angleLimit[][2] = {
  { -120, 120}, { -30, 80}, { -120, 120}, { -120, 120},
  { -90, 60}, { -90, 60}, { -90, 90}, { -90, 90},
  { -200, 80}, { -200, 80}, { -80, 200}, { -80, 200},
  { -80, 200}, { -80, 200}, { -70, 200}, { -80, 200},
};
#endif
int previousAng[DOF];
#ifdef X_LEG
int currentAng[DOF] = { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75,  75,  -75,  -75,
                        -55, -55, 55, 55
                      };
#else
int currentAng[DOF] = { -30, -80, -45, 0,
                        0, 0, 0, 0,
                        75,  75,  75,  75,
                        -55, -55, -55, -55
                      };
#endif
int zeroPosition[DOF] = {};
int calibratedZeroPosition[DOF] = {};

int8_t servoCalib[DOF] = {0, 0, 0, 0,
                          0, 0, 0, 0,
                          0, 0, 0, 0,
                          0, 0, 0, 0
                         };

int16_t imuOffset[9] = {0, 0, 0,
                        0, 0, 0,
                        0, 0, 0
                       };

//abbreviations for printing
#define PT(s) Serial.print(s)
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))//trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))
#define PRINT_SKILL_DATA

//
//template <typename T> int8_t sign(T val) {
//  return (T(0) < val) - (val < T(0));
//}

void printRange(int r0 = 0, int r1 = 0) {
  if (r1 == 0)
    for (byte i = 0; i < r0; i++) {
      PT(i);
      PT('\t');
    }
  else
    for (byte i = r0; i < r1; i++) {
      PT(i);
      PT('\t');
    }
  PTL();
}

String range2String(int r0 = 0, int r1 = 0) {
  String temp = "";
  if (r1 == 0)
    for (byte i = 0; i < r0; i++) {
      temp += i;
      temp += '\t';
    }
  else
    for (byte i = r0; i < r1; i++) {
      temp += i;
      temp += '\t';
    }
  return temp;
}

template<typename T> void printList(T *arr, byte len = DOF) {
  String temp = "";
  for (byte i = 0; i < len; i++) {
    temp += String(int(arr[i]));
    temp += ",\t";
    //PT((T)(arr[i]));
    //PT('\t');
  }
  PTL(temp);
}

template<typename T> String list2String(T *arr, byte len = DOF) {
  String temp = "";
  for (byte i = 0; i < len; i++) {
    temp += String(int(arr[i]));  //String(int(arr[i]));
    temp += ",\t";
    //PT((T)(arr[i]));
    //PT('\t');
  }
  return temp;
}

//float sign(float value) {
//  return ((value > 0) - (value < 0));
//}
template<typename T> void printTable(T *list) {
  printRange(0, DOF);
  Serial.println("HPan	HTilt	TPan	-1	-1	-1	-1	-1	SLF	SRF	SRB	SLB	KLF	KRF	KRB	KLB	");
  printList(list, DOF);
}

#include "sound.h"
#include "randomMind.h"
#ifdef ULTRASONIC
#define TRIGPIN 17 // adjust as needed: Digital pin connected to the trig pin of the ultrasonic sensor
#define ECHOPIN 17 // adjust as needed: Digital pin connected to the echo pin of the ultrasonic sensor
bool distancetriggeredW = false; // when walking
bool distancetriggeredC = false; // crawling
bool distancetriggeredT = false; // trot
bool distancetriggered_timeout = false;
int distance_timeout = 3000;
long distance_timeout_old = 0;
#include "ultraSonic_NB.h"
#endif


#include "I2cEEPROM.h"
#include "bleUart.h"
#ifdef GYRO_PIN
#include "imu.h"
#endif
#ifdef IR_PIN
#include "infrared_Cat.h"
#include "infrared_Haier.h"
#include "infrared_Generic.h"
#endif
#include "espServo.h"
#ifdef VOICE
#include "voice.h"
#endif
#ifdef CAMERA
#include "camera.h"
#endif

#include "io.h"
#include "motion.h"
#include "skill.h"
#ifdef NEOPIXEL_PIN
#include "led.h"
#endif


#include "reaction.h"
#include "qualityAssurance.h"
