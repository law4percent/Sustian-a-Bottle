/*
  Sustainabottle. 

  In this code, the Arduino Board allows to read the input object if it is
  a metal or the object has water inside on it. This is done with Inductive
  Proximity Sensor and Capacitive Proximity Sensor. Furthermore, with this
  code the internet time will update after receiving 5 times signals 
  from ESP32 camera.

  Created on 21 January 2024
  by Lawrence P. Roble

  Modified on 14 April 2013
  by Lawrence P. Roble
*/

#include <Servo.h>

Servo servo_A;
Servo servo_B;

// Pin Used
const byte echoPin = 2;
const byte trigPin = 3;
const byte LEDindcatorINPUT = 4;
const byte InternetTime = 5;
const byte servoPin_A = 6;
const byte servoPin_B = 7;
const byte ReceivedSignalFromESP = 8;
const byte OrangePi_signal = 9;
const byte ind_A = A0;
const byte ind_B = A1;
const byte cap_A = A2;
const byte cap_B = A3;
const byte LEDindicatorNotReady = 10;

void setup() {
  Serial.begin(9600);

  servo_A.attach(servoPin_A);
  servo_B.attach(servoPin_B);  
  servo_A.write(90);
  servo_B.write(50);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(LEDindcatorINPUT, OUTPUT);
  pinMode(InternetTime, OUTPUT);
  pinMode(ind_A, INPUT);
  pinMode(ind_B, INPUT);
  pinMode(cap_A, INPUT);
  pinMode(cap_B, INPUT);
  pinMode(OrangePi_signal, INPUT_PULLUP);
  pinMode(ReceivedSignalFromESP, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  digitalWrite(InternetTime, 1);
  digitalWrite(LEDindcatorINPUT, 1);
  digitalWrite(13, 1);
  delay(3000);

  while (true) {
    if (digitalRead(ReceivedSignalFromESP) == 1) {
      digitalWrite(13, 0);
      break;
    } else Serial.println("Camera is not yet ready!");
    delay(25);
  }
  digitalWrite(LEDindcatorINPUT, 0);
}

void loop() {
  bool OrangePi_state = digitalRead(OrangePi_signal);
  bool goSignal = isThereBottle();
  delay(150);


  if (!OrangePi_state and goSignal) {
    delay(2000);
    bool EvBot = EvaluateBottle();

    if (EvBot) {
      servo_A.write(0);
      delay(1000);
      servo_A.write(90);
      delay(1000);

      int countTrue = 0;
      for (int limit = 20; limit > 0; limit--) {
        delay(500);

        if (digitalRead(ReceivedSignalFromESP)) {
          countTrue++;
        }

        if (countTrue >= 5) {
          break;
        }
      }

      if (countTrue >= 5) {
        servo_B.write(180);
        digitalWrite(InternetTime, 0);
        delay(10);
        digitalWrite(InternetTime, 1);
        delay(15);
      } else {
        Warning();
        servo_B.write(0);
        delay(1000);
      }
    } else {
      Warning();
    }

    servo_B.write(50);
    delay(1000);
  }
}

void Warning() {
  digitalWrite(LEDindcatorINPUT, 1);
  delay(500);
  digitalWrite(LEDindcatorINPUT, 0);
  delay(250);
  digitalWrite(LEDindcatorINPUT, 1);
  delay(500);
  digitalWrite(LEDindcatorINPUT, 0);
}

bool isThereBottle() {
  long duration, cm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  cm = duration / 29 / 2;

  return (cm <= 8) ? true : false;
}

bool EvaluateBottle() {
  byte countDown = 3;
  int indA_val;  // val < 50 its metal
  int indB_val;  // val < 50 its metal
  int capA_val;  // val < 950 there's water
  int capB_val;  // val > 50 there's water

  while (countDown != 0) {
    indA_val = analogRead(ind_A);
    indB_val = analogRead(ind_B);
    capA_val = analogRead(cap_A);
    capB_val = analogRead(cap_B);
    delay(1000);
    countDown--;
  }

  if (capA_val < 950 or capB_val > 50 or indA_val < 50 or indB_val < 50) {
    return false;  // false means the bottle is not allow to proceed
  } else {
    return true;
  }
}
