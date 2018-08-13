
/*
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC (that is: a RFID
 * Tag or Card) using a MFRC522 based RFID Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout
 * below), load this sketch into Arduino IDE then verify/compile and upload it.
 * To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M).
 * When you present a PICC (that is: a RFID Tag or Card) at reading distance
 * of the MFRC522 Reader/PCD, the serial output will show the ID/UID, type and
 * any data blocks it can read. Note: you may see "Timeout in communication"
 * messages when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs
 * presented (that is: multiple tag reading). So if you stack two or more
 * PICCs on top of each other and present them to the reader, it will first
 * output all details of the first and then the next PICC. Note that this
 * may take some time as all data blocks are dumped, so keep the PICCs at
 * reading distance until complete.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Keyboard.h>

#define RST_PIN		9		// 
#define SS_PIN		10		//

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
int iteration_without_card = 0;
int max_iteration_without_card = 6;

//*****************************************************************************************//
void setup() {
//  Serial.begin(115200);  
//  while (!Serial);
  Keyboard.begin();
  delay(500);// Initialize serial communications with the PC
  SPI.begin();
  delay(500); // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card

}

void logout()
{
Keyboard.press(KEY_LEFT_CTRL);
Keyboard.press(KEY_LEFT_ALT);
Keyboard.print("l");
delay(100);
Keyboard.releaseAll();
delay(1000);
  
}

uint8_t iteration = 0;
uint8_t buf[10]= {};
MFRC522::Uid id;
MFRC522::Uid id2;
bool is_card_present = false;
//*****************************************************************************************//

void cpid(MFRC522::Uid *id){
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, mfrc522.uid.uidByte, mfrc522.uid.size);
  id->size = mfrc522.uid.size;
  id->sak = mfrc522.uid.sak;
}

bool cmpid(MFRC522::Uid *id1, MFRC522::Uid *id2){
  return memcmp(id1, id2, sizeof(MFRC522::Uid));
}

void deregister_card(){
  is_card_present = false;
  memset(&id,0, sizeof(id));
}

uint8_t control = 0x00;
int card_present = 0 ;
void loop() {
 
//mfrc522.PICC_HaltA();
//mfrc522.PCD_StopCrypto1();  
deregister_card();
delay(500); 
//Serial.print("Iteration:");
//Serial.println(iteration,HEX);
iteration++;


if ( !mfrc522.PICC_IsNewCardPresent()) {
 //  Serial.println("Card not present---1!");
   if( card_present != 0) { 
   iteration_without_card++;
   }
   if( card_present != 0 && iteration_without_card > max_iteration_without_card)
   {    
  //  Serial.println("Event: card removed");
    iteration_without_card=0;
    card_present = 0; 
    logout();
   }
   return;
  }
  if ( !mfrc522.PICC_ReadCardSerial()) {
 //   Serial.println("Card not present---2!");
    return;
  }
 // mfrc522.PICC_HaltA();
 // mfrc522.PCD_StopCrypto1();  
  card_present = 1;
  iteration_without_card=0;
}
