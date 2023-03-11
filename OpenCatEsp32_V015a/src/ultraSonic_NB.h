volatile unsigned long microSecondStart = 0;
volatile unsigned long microSecondEnd = 0;
#include "noneblocking.h"

#define SOUNDSPEED 343 // soundspeed expected for this area in m/s
#define MAXDISTANCE 2000 //in mm
#define MINDISTANCE 20 // in mm
//#define NOSENSOR 1 // if commented out then sensor is not available


#ifdef NOSENSOR
uint32_t testDuration = 2 * MAXDISTANCE / SOUNDSPEED * 1000; // to and back max distance, mm to meter, s to us
#endif
int distanceMM = 0;
long duration = 0;

int read_distance() {
#ifndef NOSENSOR
  if (!trigPin.activated) getDistanceMM(); // initiate measurement
  // todo timeout if echo missed

  if (echoPin.activated) {

    duration =  microSecondEnd - microSecondStart - 466;
    //if (duration % 2) duration =  microSecondEnd - microSecondStart - 1; does it make sense for integer?

    distanceMM = (duration * SOUNDSPEED) / 2000; // in millimeters:

    if (distanceMM > MAXDISTANCE) {
      distanceMM = MAXDISTANCE;
    }

    if (distanceMM < MINDISTANCE) {
      distanceMM = 0;
    }
    if (!strcmp(lastCmd, "wkF") && distanceMM < 150) { //todo WALKING
      token = T_SKILL;
      strcpy(newCmd, "bk");
      newCmdIdx = 2;
      distancetriggeredW = true;
      distancetriggered_timeout = true;
      distance_timeout_old = millis();
    }
    else if (!strcmp(lastCmd, "trF") && distanceMM < 350) { //todo TROTTING
      token = T_SKILL;
      strcpy(newCmd, "bk");
      newCmdIdx = 2;
      distancetriggeredT = true;
      distancetriggered_timeout = true;
      distance_timeout_old = millis();
    }
    else if (!strcmp(lastCmd, "crF") && distanceMM < 150) { //todo CRAWLING
      token = T_SKILL;
      strcpy(newCmd, "bk");
      newCmdIdx = 2;
      distancetriggeredC = true;
      distancetriggered_timeout = true;
      distance_timeout_old = millis();
    }
    else if ((distancetriggeredW) && (distanceMM  >= 250) && (distancetriggered_timeout)) {
      if ((distance_timeout_old + distance_timeout) < millis()) {
        token = T_SKILL;
        strcpy(newCmd, "wkF");
        newCmdIdx = 2;
        distancetriggered_timeout = false;
        distancetriggeredW = false;
      }
    }
    else if ((distancetriggeredC) && (distanceMM  >= 250) && (distancetriggered_timeout)) {
      if ((distance_timeout_old + distance_timeout) < millis()) {
        token = T_SKILL;
        strcpy(newCmd, "ck");
        newCmdIdx = 2;
        distancetriggered_timeout = false;
        distancetriggeredC = false;
      }
    }
    else if ((distancetriggeredT) && (distanceMM  >= 350) && (distancetriggered_timeout)) {
      if ((distance_timeout_old + distance_timeout) < millis()) {
        token = T_SKILL;
        strcpy(newCmd, "wkF");
        newCmdIdx = 2;
        distancetriggered_timeout = false;
        distancetriggeredT = false;
      }
    }
    /* 	Serial.print(distanceMM);
        Serial.println(" mm: distanceMM"); */
    //randomInterval = 5000;
    echoPin.activated = false;
    trigPin.activated = false;
  }

#elif defined NOSENSOR

  duration  = random(testDuration);

  distanceMM = (duration * SOUNDSPEED) / 2000; // in millimeters:

  if (distanceMM > MAXDISTANCE) {
    distanceMM = MAXDISTANCE;
  }

  if (distanceMM < MINDISTANCE) {
    distanceMM = 0;
  }
  Serial.print(distanceMM); //(distanceMM);
  Serial.println(" mm - RANDOM!");
  echoPin.activated = false;
  trigPin.activated = false;
#endif
  return distanceMM; //however, distanceMM is anyway global
}

/*
    loadBySkillName("hi");
    skill->perform();
  action if distance:

  long ultraTimer;
  int ultraInterval = 1000;
  int distance;

  void read_ultrasonic() {
  if  (millis() - ultraTimer > ultraInterval || token == 'k' && millis() - ultraTimer > 3000) { //or trig set
    ultraTimer = millis();
    ultraInterval = 0;
    randomInterval = 1000;
    //distance = mRUS04.GetUltrasonicDistance();
      Serial.print(distance); //(distanceMM);
    Serial.println(" cm");

     if (distance == 640) {
      return;
    }

    if (distance > 60) {
      //mRUS04.SetRgbEffect(E_RGB_ALL, RGB_WHITE, E_EFFECT_BREATHING);
      ultraInterval = 1000;
  //      autoSwitch = true;
      randomInterval = 1000;
    }
    else if (distance > 40) {
     // mRUS04.SetRgbEffect(E_RGB_ALL, RGB_YELLOW, E_EFFECT_ROTATE);
    }
    else if (distance < 2) {
      token = T_SKILL;
      strcpy(newCmd, "bk");
      newCmdIdx = 6;
      ultraInterval = 2000;
      randomInterval = 5000;
    }
    else if (distance < 4) {
      //mRUS04.SetRgbEffect(E_RGB_ALL, RGB_RED, E_EFFECT_FLASH);
      meow(rand() % 3 + 1, distance * 2);
      token = T_INDEXED_SIMULTANEOUS_BIN;
      char allRand[] = {0, currentAng[0] + rand() % 20 - 10, 1, currentAng[1] + rand() % 20 - 10, 2, currentAng[2] + rand() % 80 - 40
                       };
      cmdLen = 6;
      for (byte i = 0; i < cmdLen; i++)
        dataBuffer[i] = allRand[i];
      dataBuffer[cmdLen] = '\0';
      newCmdIdx = 6;
    }
    else if (distance < 6) {
      //mRUS04.SetRgbColor(E_RGB_ALL, RGB_RED);
      token = 'k';
      strcpy(newCmd, "sit");
      newCmdIdx = 6;
    }

    else { //6~40
      distance -= 6;
      //mRUS04.SetRgbColor(E_RGB_ALL, colors[max(min(distance / 7, 5), 0)]);
      token = T_LISTED_BIN;
      int mid[] = {0,   0,   0,   0,   0,   0,   0,   0,  30,  30, -30, -30,  30,  30, -30, -30,};
      int allParameter[] = { currentAng[0] * 2 / 3 - distance / 2, -10 + currentAng[1] * 2 / 3 + distance / 1.5 , (distance * 3  - 50)*(rand() % 50 < 1 ? rand() % 2 - 1 : 1), 0,
                             0, 0, 0, 0,
                             mid[8] - 15 + distance / 2 , mid[9] - 15 + distance / 2, mid[10] - 30 + distance, mid[11] - 30 + distance,
                             mid[12] + 35 - distance, mid[13] + 35 - distance, mid[14] + 40 - distance, mid[15] + 40 - distance
                           };
      //      printList(allParameter);
      cmdLen = 16;
      for (byte i = 0; i < cmdLen; i++)
        dataBuffer[i] = (int8_t)min(max(allParameter[i], -128), 127);
      dataBuffer[cmdLen] = '\0';
      newCmdIdx = 6;
      randomInterval = 5000;
    }
  }
  }*/
