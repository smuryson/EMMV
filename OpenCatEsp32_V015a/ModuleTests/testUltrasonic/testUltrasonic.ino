// defines pins numbers
#define TRIGGER 17
#define ECHO 17
//#define BUZZER 5
// defines variables
long duration;
int distance;

#define LONGEST_DISTANCE 200 // 200 cm = 2 meters
float farTime =  LONGEST_DISTANCE * 2 / 0.034;

long counter = 0;

void setup() {
  pinMode(TRIGGER, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200); // Starts the serial communication
}

void loop() {
  // Clears the trigPin
  pinMode(TRIGGER, OUTPUT);
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIGGER, HIGH);
  //digitalWrite(BUZZER,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  //digitalWrite(BUZZER,LOW);
  pinMode(ECHO, INPUT);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO, HIGH, farTime);
  Serial.println(counter++);


  // Calculating the distance
  distance = duration * 0.034 / 2; // 10^-6 * 34000 cm/s

  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
}
