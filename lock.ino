
/* ----------------------------------------------------------------------------
 * RFID PC Locker
 * origin: https://github.com/vitos73/rfidpclocker
 * Feel free to use and modify under GNU GPL v3
 * (c)2018  Vitold Buttermaan
 * ----------------------------------------------------------------------------
 * This script based on a MFRC522 library ; see https://github.com/miguelbalboa/rfid
 * Used hardware:
 * Reader - https://arduino.ua/prod2079-rfid-modyl-rc522-ot-robotdyn
 * Board  - https://arduino.ua/prod661-arduino-pro-micro-s-konnektorami
 * 
 * Important note: This script works only with boards based on Atmega32U4!
 * otherwise, you can't send any event to pc as HID!
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     9
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Keyboard.h>

#define RST_PIN		9	 // connect reader reset to 9 pin (not for reset on arduino board). MFRC522 library use reset to wakeup the reader.
#define SS_PIN		10 //

MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance
uint8_t iteration_without_card = 0;      // init value for iteration counter
uint8_t max_iteration_without_card = 6;  // this value is for protection from false positive - sometimes reader losts communication with card
bool card_present = false;

//*****************************************************************************************//
void setup() {
  Keyboard.begin();
  delay(500);// delay for init
  SPI.begin();
  delay(500); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

}

void logout()
{
  // send key kombination for lock PC - this for Ubuntu (ctrl+alt+l)
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.print("l");
  delay(100);
  Keyboard.releaseAll();
  delay(1000);  
}


void loop() {

    delay(500); // you can change this value to detetect card removal faster 

    if ( !mfrc522.PICC_IsNewCardPresent()) {
           if( card_present) { 
              iteration_without_card++;
           }
           if( card_present && iteration_without_card > max_iteration_without_card) {
              // we can't see card long time    
              iteration_without_card=0;
              card_present = false; 
              logout(); // sends logout event to PC
           }
           return;
      }
    if ( !mfrc522.PICC_ReadCardSerial()) {
        return;
      }

// this lines reached only if reader see any card on it 

    card_present = true;
    iteration_without_card=0;
}
