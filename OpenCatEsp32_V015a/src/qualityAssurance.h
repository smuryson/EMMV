float mean(float *a, int n) {
  float sum = 0;
  for (int i = 0; i < n; i++)
    sum += a[i];
  return sum / n;
}

float sDev(float *a, float m, int n) {
  float sum = 0;
  for (int i = 0; i < n; i++)
    sum += (a[i] - m) * (a[i] - m);
  return sqrt(sum / n);
}
byte mpuGood[] = { 12, 16, 19,
                   4, 4, 4
                 };
byte mpuBad[] = { 19, 17, 16, 14, 12,
                  16, 16, 16, 16, 16
                };

#define MEAN_THRESHOLD 0.2
#define STD_THRESHOLD 0.02

void testMPU() {
  PTL("\nIMU test: both mean and standard deviation should be small on Pitch and Roll axis\n");
  delay(1000);
  int count = 50;
  float **history = new float *[2];
  for (int a = 0; a < 2; a++)
    history[a] = new float[count];
  for (int t = 0; t < count; t++) {
    delay(5);
    read_IMU();
    print6Axis();
    for (int a = 0; a < 2; a++)
      history[a][t] = ypr[a + 1];
  }
  String axis[] = { "Pitch ", "Roll  " };
  for (int a = 0; a < 2; a++) {
    float m = mean(history[a], count);
    float dev = sDev(history[a], m, count);
    PT(axis[a]);
    PT("(in degrees)\tmean: ");
    PT(m);
    PT("\tstandard deviation: ");
    PT(dev);
    if (fabs(m) > MEAN_THRESHOLD || dev > STD_THRESHOLD) {
      PTL("\tFail!");
      while (1) {
        playMelody(mpuBad, sizeof(mpuBad) / 2);
        delay(500);
      }
    } else {
      PTL("\tPass!");
      playMelody(mpuGood, sizeof(mpuGood) / 2);
    }
  }
  delay(100);
  for (int a = 0; a < 2; a++)
    delete[] history[a];
  delete[] history;
};
#ifdef IR_PIN
bool testIR() {
  long start = millis();
  long timer = start;
  int count = 0, right = 0;
  int current = 0;
  int previous = 10;
  bool pass = false;
  PTL("\nInfrared test: catch at least 6 consecutive signals\n");
  while (1) {
    if (count == 10 || millis() - start > 1200 || right > 5) {  //test for 1 second
      PT(right);
      PT("/");
      PT(count);
      PTL(" good");
      if (right > 5)
        return true;
      else
        return false;
    }

    if (millis() - timer > 11 && irrecv.decode(&results)) {
      timer = millis();
      current = IRkey();
      irrecv.resume();  // receive the next value
      if (current == 0)
        continue;

      if (current == 11)
        previous = 10;
      if (current - previous == 1)  //if the reading is continuous, add one to right
        right++;
      PT("count");
      PT(count);
      PT("\tprevious ");
      PT(previous);
      PT("\tcurrent ");
      PT(current);
      PT("\tright ");
      PTL(right);
      previous = (current == 20) ? 10 : current;
      count++;
      //      beep(current, 10);
    }
  }
}
#endif
void QA() {
  if (newBoard) {
#ifndef AUTO_INIT
    PTL("Run factory quality assurance program? (Y/n)");
    while (!Serial.available())
      ;
    char choice = Serial.read();
    PTL(choice);
    if (choice != 'Y' && choice != 'y')
      return;
#endif
    testMPU();
    //tests...
    PTL("\nServo test: all servos should rotate and in sync\n");
    while (1) {
      //loadBySkillName("rest");  //test EEPROM, org "ts" available in instictBittleESP.h NOT in instictNybbleESP.h !!!
      //skill->perform(); //todo that kills the cat not the dog as it was not available
#ifdef IR_PIN
      if (testIR()) {
#endif
        PTL("Pass");
        playMelody(melodyIRpass, sizeof(melodyIRpass) / 2);
        break;
#ifdef IR_PIN
      } else {
        PTL("Fail ir");
        beep(8, 50);
      }
#endif
    }
  }
}
