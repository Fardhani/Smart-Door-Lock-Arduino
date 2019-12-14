/*
 * -----------------------------------
 *             MFRC522      NodeMCU    
 *             Reader/PCD             
 * Signal      Pin                    
 * -----------------------------------
 * RST/Reset   RST          D1
 * SPI SS      SDA(SS)      D2
 * SPI MOSI    MOSI         D7
 * SPI MISO    MISO         D6
 * SPI SCK     SCK          D5
 */

/*
 * Relay
 * D0
 */

#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = D1;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D2;      // Configurable, see typical pin layout above
constexpr uint8_t RELAY = D0;       // Configurable, see typical pin layout above
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
//condition lock door
boolean isLocked = true;

void setup() { 
  pinMode(RELAY, OUTPUT); //initialize relay
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
 
void loop() {
  //set kondisi relay
  if(isLocked){
    //relay dimatikan
    digitalWrite(RELAY, LOW);
  }
  else{
    digitalWrite(RELAY, HIGH);
  }

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  Serial.println(F("A new card has been detected."));

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  //compare RFID Tag with Registered UID
  if(nuidPICC[0] == 182 && nuidPICC[1] == 118 && nuidPICC[2] == 186 && nuidPICC[3] == 247){
    isLocked = false;
  }
  else{
    isLocked = true;
  }
 
  Serial.println(F("The NUID tag is:"));
  Serial.print(F("In hex: "));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  Serial.print(F("In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
