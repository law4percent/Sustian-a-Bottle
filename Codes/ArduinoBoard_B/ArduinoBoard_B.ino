/*
  Sustainabottle.

  In this code, the Arduino Board allows to evaluate the consumed capacity storage
  of the project and send updates into the user's phone number with help of
  Sim900A GSM module.

  Created on 2 April 2024
  by Lawrence P. Roble
*/
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial SIM900(9, 10);  // RX, TX pins
#define PhoneNumberLength 11
#define EEPROM_ADDRESS 0

String receivedString = "";
#define trigPin 2
#define echoPin 3
#define relay 4
#define LightIndicator 13

String recipientNum = "";
bool goSignal_80 = true;
bool goSignal_95 = true;
bool CRNum_MODE = false;

void setup() {
  // Uncomment this if want to change new recipient number
  // changePhoneNumber("09167212615");

  for (int i = 0; i < PhoneNumberLength; i++) {
    recipientNum += char(EEPROM.read(EEPROM_ADDRESS + i));
  }

  Serial.begin(115200);
  SIM900.begin(115200);
  delay(1000);

  pinMode(trigPin, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LightIndicator, OUTPUT);
  digitalWrite(relay, 1);
  digitalWrite(LightIndicator, 0);

  Serial.println("Initializing SIM900A...");
  SIM900.println("AT");
  delay(1000);

  if (SIM900.find("OK")) {

    // Set SMS mode to text mode
    SIM900.println("AT+CMGF=1");
    delay(1000);
    // Enable SMS text mode
    SIM900.println("AT+CNMI=2,2,0,0,0");
    delay(1000);
    SIM900.println("AT+CMGD=1,4");  // delete all SMS
    delay(5000);
    Serial.println("SIM900A is ready.");
  } else {
    Serial.println("SIM900A initialization failed. Check connections and restart.");
    while (1) {
      digitalWrite(LightIndicator, 1);
      delay(1000);
      digitalWrite(LightIndicator, 0);
      delay(1000);
    }
  }
}

void loop() {
  String theMessage;
  long distance = Distance();
  int count = 0;
  digitalWrite(LightIndicator, 0);
  Serial.print("distance: ");
  Serial.println(distance);

  if (distance < 0) {
    distance = 64;
  } else if (distance < 5) {
    distance = 5;
  } else if (distance > 64) {
    distance = 64;
  }

  int levelCapacity = map(distance, 64, 5, 0, 100);  // returning value from 0 to 100 as percentage val
  Serial.print("Percentage: ");
  Serial.print(levelCapacity);
  Serial.println("%");

  if (levelCapacity >= 95 and goSignal_95) {
    digitalWrite(relay, 0);
    goSignal_95 = false;
    goSignal_80 = false;
    theMessage = "Warning! The capacity is now reaching at ";
    theMessage = theMessage + levelCapacity;
    theMessage = theMessage + "% of consumed storage. Thank you!";
    sendSMS(recipientNum, theMessage);
  } else if (levelCapacity >= 80 and goSignal_80) {
    goSignal_80 = false;
    theMessage = "Hello there! Just reminding you that the capacity is now reaching at ";
    theMessage = theMessage + levelCapacity;
    theMessage = theMessage + "% of consumed storage. Thank you!";
    sendSMS(recipientNum, theMessage);
  }

  if (levelCapacity < 10 and !goSignal_95) {
    goSignal_80 = true;
    goSignal_95 = true;
    digitalWrite(relay, 1);
  }



  while (SIM900.available() > 0) {
    char c = SIM900.read();
    receivedString += c;

    if (c == '\n' || c == '\r') {
      receivedString.trim();
      if (receivedString.equals("CRNum")) {
        CRNum_MODE = true;
        sendSMS(recipientNum, "Please reply the new number '09XXXXXXXXX_' by this format. To keep the current number reply 'Cancel'.");
      } else if (receivedString.equals("CapStats")) {
        theMessage = "Capacity Level: ";
        theMessage = theMessage + levelCapacity;
        theMessage = theMessage + "% of consumed storage";
        sendSMS(recipientNum, theMessage);
      }
      receivedString = "";
    }
  }


  String firstWord, secondWord;

  while (CRNum_MODE and count < 250) {
    count++;
    digitalWrite(LightIndicator, 1);
    Serial.println("waiting..");

    if (SIM900.available() > 0) {
      char ch = SIM900.read();
      receivedString += ch;

      if (ch == '\n' || ch == '\r') {
        receivedString.trim();
        Serial.println(receivedString);
        delay(2000);

        if (receivedString.equals("Cancel")) {
          sendSMS(recipientNum, "The cancellation has approved. Thank you!");
          CRNum_MODE = false;
        }


        if (receivedString.length() > 11 and receivedString[0] == '0' and receivedString[1] == '9' and receivedString[11] == '_') {
          char xCh;
          int i = 0;
          String numData = "";

          while (receivedString.length() > 11 and i < 11) {
            xCh = receivedString[i];
            numData += xCh;
            i++;
          }

          if (numData.length() == 11) {
            Serial.println(numData);
            delay(2000);
            recipientNum = numData;
          }

          sendSMS(recipientNum, "Transferring to another mobile number has been successful.");
          CRNum_MODE = false;
        }

        receivedString = "";
      }
    }
    delay(250);
  }
  delay(250);
}


void sendSMS(String phoneNumber, String message) {
  digitalWrite(LightIndicator, 1);

  // Set SMS mode to text mode
  SIM900.println("AT+CMGF=1");
  delay(1000);

  // Specify the recipient phone number
  SIM900.print("AT+CMGS=\"");
  SIM900.print(phoneNumber);
  SIM900.println("\"");
  delay(1000);

  // Send the SMS message
  SIM900.print(message);
  delay(5000);
  SIM900.println((char)26);  // ASCII code for Ctrl+Z
  delay(1000);

  // Print the SMS status
  if (SIM900.find("OK")) {
    Serial.println("SMS sent successfully.");
  } else {
    Serial.println("Error sending SMS.");
  }

  digitalWrite(LightIndicator, 0);
}

long Distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  return duration / 29 / 2;
}

void changePhoneNumber(const char* newPhoneNumber) {
  for (int i = 0; i < PhoneNumberLength; i++) {
    EEPROM.write(EEPROM_ADDRESS + i, newPhoneNumber[i]);
  }
}