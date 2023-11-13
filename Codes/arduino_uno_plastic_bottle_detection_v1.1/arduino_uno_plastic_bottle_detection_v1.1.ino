const byte InsertCoinGoSignal = 13;
bool Check_InsertCoinGoSignal = false;

#include <Servo.h>
Servo servo[4];
const byte servoCoinSlot = 11;
const byte servoEntrance = 10;
const byte servoGoBottom = 9;
const byte servoGoLeft   = 8;
int angle = 0;

const byte IndSensor = A0;
const byte CapSensor = A1;
int Check_IndSensor;
int Check_CapSensor;

const byte trig = 7; 
const byte echo = A2;
int duration, cm;

void Servos(byte location);
void Angle(byte loc, int sec);
bool BottleStatus();
bool BottleShapeStatus();

const byte SignalToESPcam = 2;
const byte ShapeLikeBottle = 3;
const byte NotBottle = 4;
bool Check_ShapeLikeBottle;
bool Check_NotBottle;

void setup() {
  Serial.begin(9600);
  pinMode(InsertCoinGoSignal, INPUT);
  servo[0].attach(servoEntrance);
  servo[1].attach(servoGoBottom);
  servo[2].attach(servoGoLeft);
  servo[3].attach(servoCoinSlot);
  pinMode(IndSensor, INPUT);
  pinMode(CapSensor, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  
  pinMode(SignalToESPcam,  OUTPUT);
  pinMode(ShapeLikeBottle, INPUT);
  pinMode(NotBottle,       INPUT);
  digitalWrite(SignalToESPcam, 0);
  Serial.println("Please wait for 3 secs... Preparing...");
  delay(3000);
}

void loop() {
  Check_InsertCoinGoSignal = digitalRead(InsertCoinGoSignal);
  Serial.print("InsertCoinGoSignal_Status: ");
  Serial.println(Check_InsertCoinGoSignal);
  
  if (Check_InsertCoinGoSignal) {
    digitalWrite(trig, LOW);
  	delayMicroseconds(2);
  	digitalWrite(trig, HIGH);
  	delayMicroseconds(10);
  	digitalWrite(trig, LOW);
  	duration = pulseIn(echo, HIGH);
    
    if(cm < 150) {
      if (BottleStatus() == false) {
        Servos(servoGoBottom);
      } else {
        digitalWrite(SignalToESPcam, 1);
        delay(3000);
        if (BottleShapeStatus() == false) {
          Servos(servoGoBottom);
        } else {
          Servos(servoGoLeft);
          Servos(servoCoinSlot);
        }
        digitalWrite(SignalToESPcam, 0);
      }
    }
  }
  Serial.println();
}

void Servos(byte location) {
  switch (location) {
    case servoCoinSlot:
      Angle(3, 0);
      break;
    case servoGoLeft:
      Angle(2, 1000);
      break;
    case servoGoBottom:
      Angle(1, 1000);
      break;
    case servoEntrance:
      Angle(0, 0);
      break;
  }
}

void Angle(byte loc, int sec) {
  delay(sec);
  
  for(angle = 0; angle < 180; angle++) {
    servo[loc].write(angle);
    delay(10);
  }
  
  for(angle = 180; angle > 0; angle--) {
    servo[loc].write(angle);
    delay(10);
  }
}

/*** Check if it is a Plastic and if it has water ***/
bool BottleStatus() {
  byte countDown = 3;
  bool bStatus;

  while (countDown != 0) {
  	Check_CapSensor = analogRead(CapSensor);
  	Check_IndSensor = analogRead(IndSensor);
  	delay(500);
    countDown--;
  }
  
  Serial.print("CapSensor: ");
  Serial.print(Check_CapSensor);
  Serial.print("  &&  IndSensor: ");
  Serial.println(Check_IndSensor);
  
  if (Check_CapSensor < 25 && Check_IndSensor > 25) {
    bStatus = true;
  } else {
    bStatus = false;
  }
  
  Servos(servoEntrance);
  return bStatus;
}

bool BottleShapeStatus() {
  Check_ShapeLikeBottle = digitalRead(ShapeLikeBottle);
  Check_NotBottle = digitalRead(NotBottle);
  Serial.print("ShapeLikeBottle: ");
  Serial.print(Check_ShapeLikeBottle);
  Serial.print("  &&  NotBottle: ");
  Serial.println(Check_NotBottle);
  
  if (Check_ShapeLikeBottle && !Check_NotBottle) {
    return true;
  } else {
    return false;
  }
}
