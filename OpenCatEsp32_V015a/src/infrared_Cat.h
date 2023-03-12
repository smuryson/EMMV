
//#include "ir/IRremote.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

//#include <IRremote.h>
//The included library is identical to the IRremote library by shirriff, version 2.6.1
//Source: https://github.com/Arduino-IRremote/Arduino-IRremote
//Here, we include the decoding functions in our folder only to make it more convenient for newbie users
//All rights belong to the original author, and we follow the MIT license.
//You no longer need to modify ~/Documents/Arduino/libraries/src/IRremote/IRremote.h as mentioned in our old manual.

IRrecv irrecv(IR_PIN); //PIN 14

decode_results results;
//abbreviation //gait/posture/function names
#define K01 "F"         //forward
#define K02 "g"         //turn off gyro feedback to boost speed
#define K03 "buttUp"    //butt up :-) //ADDED

#define K05 "lu"        //look up
#define K06 "ck"        //check around

#define K10 "L"         //left
//#define K11 "balance" //neutral stand up posture
#define K11 "d"         //rest and shutdown all servos
#define K12 "R"         //right

#define K20 ","         //pause motion and shut off all servos
#define K21 "B"         //backward
#define K22 "."         //calibration mode with IMU turned off

#define K30 "vt"        //stepping
#define K31 "cr"        //crawl
#define K32 "wk"        //walk

#define K40 "tr"        //trot
#define K41 "sit"       //sit
#define K42 "str"       //stretch

#define K50 "hi"        //greeting
#define K51 "pu"        //push up
#define K52 "pee"       //standng with three legs

#ifdef NYBBLE
#define K60 "g"         //"gyro"
#define K61 "z"         //random
#elif defined BITTLE
#define K60 "ck"        //check around
#define K61 "bf"
#else
#define K60 "fd"        //fold
#define K61 "rt"
#endif

#define K62 "c"         //calibrate

//#define SHORT_ENCODING // activating this line will use a shorter encoding of the HEX values

String translateIR() {
  // takes action based on IR code received
  // describing Remote IR codes.
  // print() & println() can't handle printing long longs. (uint64_t)
  Serial.print("translateIR(): ");
  serialPrintUint64(results.value, HEX);
  Serial.println("");
  //#ifndef SHORT_ENCODING

  //BUTTON DISTRIBUTION ON REMOTE
  switch (results.value) {
    //IR signal    key on IR remote         //key mapping
    case 0xFFA25D: PTLF(" 1");              return (K50); //say hi                                    //remote 1
    case 0xFF629D: PTLF(" 2");              return (K41); //sit                                       //remote 2
    case 0xFFE21D: PTLF(" 3");              return (K42); //stretch                                   //remote 3

    case 0xFF22DD: PTLF(" 4");              return (K51); //push up                                   //remote 4
    case 0xFF02FD: PTLF(" 5");              return (K52); //pee                                       //remote 5
    case 0xFFC23D: PTLF(" 6");              return (K03); //butt up                                   //remote 6

    case 0xFFE01F: PTLF(" 7");              return (K31); //crawl                                     //remote 7                 
    case 0xFFA857: PTLF(" 8");              return (K32); //walk                                      //remote 8
    case 0xFF906F: PTLF(" 9");              return (K40); //trot                                      //remote 9

    case 0xFF6897: PTLF(" *");              return (K61); //random                                    //remote *
    case 0xFF9867: PTLF(" 0");              return (K06); //check around                              //remote 0
    case 0xFFB04F: PTLF(" #");              return (K62); //calibrate                                 //remote #

    //case 0xFF30CF: PTLF(" n/a");          return (K40); //ok changed//trot
    case 0xFF18E7: PTLF(" UP");             return (K01); //forward                                   //remote UP
    //case 0xFF7A85: PTLF(" n/a");          return (K42); //ok changed//stretch

    case 0xFF10EF: PTLF(" LEFT");           return (K10); //left                                      //remote LEFT
    case 0xFF38C7: PTLF(" OK");             return (K11); //rest                                      //remote OK
    case 0xFF5AA5: PTLF(" RIGHT");          return (K12); //right                                     //remote RIGHT

    //case 0xFF42BD: PTLF(" n/a");          return (K60); //ok changed//gyro -> balance sensor
    case 0xFF4AB5: PTLF(" DOWN");           return (K05); //look up                                   //remote DOWN
    //case 0xFF52AD: PTLF(" n/a");          return (K62); //ok changed//calibrate

    case 0xFFFFFFFF: return ("");           //Serial.println(" REPEAT");

    //with if else check for certain time more combinations possibble -> e.g. within 0.5 seconds 1 and 1 would amount to an 11

    default: {
        //Serial.println(results.value, HEX);
      }
      return ("");                          //Serial.println("null");
  }// End Case
  //delay(100); // Do not get immediate repeat //no need because the main loop is slow
}

String gait = "wk";
char direct = 'F';

String irParser(String raw) {
  if (raw == "B") {
    gait = "bk";//(gait == "vt") ? "bk" : "vt";
    return gait;
  }
  if (raw == "F" || raw == "L" || raw == "R") {
    direct = raw[0];
    if (direct == 'F') {
      //      if (gait == "bk")
      //        gait = "vt";
      //      else if (gait == "vt")
      gait = "tr";
    }
    return gait + direct;
  }
  //  if (raw == "B") {
  //    if ( gait == "bk")
  //      runDelay = max(runDelay - delayStep, delayShort);
  //    else {
  //      runDelay = min(runDelay + delayStep, delayLong);
  //      if (runDelay == delayLong) {
  //        gait = (gait == "vt") ? "bk" : "vt";
  //        runDelay = delayMid;
  //      }
  //    }
  //    PTL(gait + direct);
  //    if (gait == "bk")
  //      return gait;
  //    else
  //      return gait + direct;
  //  }
  //  if (raw == "F" || raw == "L" || raw == "R") {
  //    direct = raw[0];
  //    if (direct == 'F') {
  //      if ( gait == "bk") {
  //        runDelay = min(runDelay + delayStep, delayLong);
  //        if (runDelay == delayLong) {
  //          //          runDelay = delayMid;
  //          gait = "vt";
  //          return gait;
  //        }
  //      }
  //      else {
  //        runDelay = max(runDelay - delayStep, delayShort);
  //        if (runDelay == delayShort && gait == "vt") {
  //          //          runDelay = delayMid;
  //          gait = "wk";
  //        }
  //      }
  //    }
  //    return gait + direct;
  //  }
  else if (raw == "vt" || raw == "cr" || raw == "wk" || raw == "mh" || raw == "tr" || raw == "rn" || raw == "bd" || raw == "ph") {
    gait = raw;
    return gait + direct;
  }
  else {
    walkingQ = false;
    return raw;
  }
}

void read_infrared() {
  if (irrecv.decode(&results)) {
    String IRsig = irParser(translateIR());
    //PTL(IRsig);
    if (IRsig != "") {
      strcpy(newCmd, IRsig.c_str());
      if (strlen(newCmd) == 1)
        token = newCmd[0];
      else
        token = T_SKILL;
      newCmdIdx = 1;
      //      PTL(String(token) + "\t" + String(gait) + "\t" + String(direct) + "\tCmd: " + String(newCmd) + "\t" + String(runDelay));
    }
    irrecv.resume(); // receive the next value
    //delay(350); //todo added as usually FFFFFFFF received, but should be none blocking
  }
}

//for QA
int IRkey() {
  // takes action based on IR code received
  // describing Remote IR codes.
  switch (results.value) {
    case 0xFFA25D: /*PTLF(" CH-");   */       return (2); //remote 1
    case 0xFF629D: /*PTLF(" CH");  */         return (3); //remote 2
    case 0xFFE21D: /*PTLF(" CH+"); */         return (4); //remote 3

    case 0xFF22DD: /*PTLF(" |<<"); */         return (5); //remote 4
    case 0xFF02FD: /*PTLF(" >>|"); */         return (6); //remote 5
    case 0xFFC23D: /*PTLF(" >||"); */         return (7); //remote 6

    case 0xFFE01F: /*PTLF(" -");   */         return (8); //remote 7
    case 0xFFA857: /*PTLF(" +");  */          return (9); //remote 8
    case 0xFF906F: /*PTLF(" EQ"); */          return (10); //remote 9

    case 0xFF6897: /*PTLF(" 0");  */          return (11); //remote *
    case 0xFF9867: /*PTLF(" 100+"); */        return (12); //remote 0
    case 0xFFB04F: /*PTLF(" 200+"); */        return (13); //remote #

    case 0xFF30CF: /*PTLF(" 1");  */          return (14); //--
    case 0xFF18E7: /*PTLF(" 2");  */          return (15); //remote UP
    case 0xFF7A85: /*PTLF(" 3");  */          return (16); //--

    case 0xFF10EF: /*PTLF(" 4");  */          return (17); //remote LEFT
    case 0xFF38C7: /*PTLF(" 5");  */          return (18); //remote OK
    case 0xFF5AA5: /*PTLF(" 6");  */          return (19); //remote RIGHT

    case 0xFF42BD: /*PTLF(" 7");  */          return (20); //--
    case 0xFF4AB5: /*PTLF(" 8");  */          return (21); //remote DOWN
    case 0xFF52AD: /*PTLF(" 9");  */          return (22); //--

    case 0xFFFFFFFF: return (-1); //Serial.println(" REPEAT"); */

    default: {
        //Serial.println(results.value, HEX);
      }
      return (0);                      //Serial.println("null");
  }// End Case
  // //delay(100); // Do not get immediate repeat //no need because the main loop is slow
}
