/*
   -----------------------------------
               MFRC522      NodeMCU
               Reader/PCD
   Signal      Pin
   -----------------------------------
   RST/Reset   RST          D1
   SPI SS      SDA(SS)      D2
   SPI MOSI    MOSI         D7
   SPI MISO    MISO         D6
   SPI SCK     SCK          D5
*/

/*
   Relay
   D8
   5v
   GND
*/

/*
  Magnetic Switch
  D0
  GND
*/

#include <SPI.h>
#include <MFRC522.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#define WIFI_SSID "Xperia Z5 Dual"
#define WIFI_PASSWORD "fardhanimusvian"
#define FRTDB "smart-home-2fb17.firebaseio.com"
#define FRTDBKey "pku0VH4TNNdP8hsehesRvC9bo5CZbYZ8VxcgWpPT"
constexpr uint8_t RST_PIN = D1;
constexpr uint8_t SDA_SS_PIN = D2;
constexpr uint8_t RELAY = D8;
constexpr uint8_t DOOR_SWITCH = D0;

//RFID Reader
MFRC522 rfid(SDA_SS_PIN, RST_PIN);

// Init array that will store new uid
byte uidRFID[4];

//Declare the Firebase Data object
FirebaseData firebaseData;

//condition lock door
boolean isLocked = true;
boolean isClosed = true;
boolean isSecurityMode = true;
String storedUid[2] = {" b6 76 ba f7", ""};
String readUid = "";

void setup() {
  Serial.begin(9600);
  SPI.begin();

  //connect wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("connected");
  Serial.println(WiFi.localIP());

  //Setup Firebase credential
  Firebase.begin(FRTDB, FRTDBKey);

  //Optional, set AP reconnection
  //  Firebase.reconnectWiFi(true);

  //Optional, set number of error retry
  Firebase.setMaxRetry(firebaseData, 3);

  //Optional, set number of error resumable queues
  Firebase.setMaxErrorQueue(firebaseData, 30);

  //door switch as output
  pinMode(DOOR_SWITCH, OUTPUT);
  //relay as output
  pinMode(RELAY, OUTPUT);
  //initialize RFID
  rfid.PCD_Init();
  //set door switch
  digitalWrite(DOOR_SWITCH, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    //read registered uid
    //    if (Firebase.stringData(firebaseData, "/key")) {
    //      /*
    //         belum kelar
    //      */
    //      Serial.println(firebaseData);
    //
    //    } else {
    //      Serial.println(firebaseData.errorReason());
    //    }
    //read isLocked
    if (Firebase.getBool(firebaseData, "/security_status/locked")) {
      if (firebaseData.dataType() == "boolean") {
        isLocked = firebaseData.boolData();
        Serial.println(firebaseData.boolData());
      }

    } else {
      Serial.println(firebaseData.errorReason());
    }
    //read security mode
    if (Firebase.getBool(firebaseData, "/security_status/securityMode")) {
      if (firebaseData.dataType() == "boolean") {
        isSecurityMode = firebaseData.boolData();
        Serial.println(firebaseData.boolData());
      }

    } else {
      Serial.println(firebaseData.errorReason());
    }

    //check condition of magnetic door switch
    if (digitalRead(DOOR_SWITCH) == HIGH) {
      if (isClosed == true) {
        isClosed = false;
        setFirebaseClosed(isClosed);
      }
    }
    else {
      if (isClosed == false) {
        isClosed = true;
        setFirebaseClosed(isClosed);
      }
    }

    // Look for new cards
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.println(rfid.PICC_GetTypeName(piccType));

      Serial.println(F("A new card has been detected."));

      //store uid to registered
      readUid = "";
      for (byte i = 0; i < rfid.uid.size; i++)
      {
        readUid.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
        readUid.concat(String(rfid.uid.uidByte[i], HEX));
      }
      Serial.println(readUid);

      //compare RFID Tag with Registered UID
      if (storedUid[0] == readUid) {
        isLocked = false;
        if (Firebase.setBool(firebaseData, "/security_status/locked", isLocked))
        {
          Serial.println("PASSED");
        }
        else
        {
          Serial.println(firebaseData.errorReason());
        }
        /*
           2. push to firebase in activity
        */
      }
      else{
        isLocked = true;
        if (Firebase.setBool(firebaseData, "/security_status/locked", isLocked))
        {
          Serial.println("PASSED");
        }
        else
        {
          Serial.println(firebaseData.errorReason());
        }
      }

      // Halt PICC
      rfid.PICC_HaltA();

      // Stop encryption on PCD
      rfid.PCD_StopCrypto1();

    }

    //check locked
    if (isLocked) {
      //set relay to off
      digitalWrite(RELAY, LOW);
    }
    else {
      //set relay to on
      digitalWrite(RELAY, HIGH);
    }
  }
}

void setFirebaseClosed(bool isClosed) {
  if (Firebase.setBool(firebaseData, "/security_status/closed", isClosed))
  {
    Serial.println("PASSED CLOSED OR OPENED");
  }
  else
  {
    Serial.println(firebaseData.errorReason());
  }
}
