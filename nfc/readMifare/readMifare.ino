
#include <Wire.h>
#include <Adafruit_PN532.h>


#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  Serial.print(versiondata);
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
}

int getBlockNumber(int sector, int block) {
  int numBlock;
  if (sector == 0) {
    if (block == 0 || block == 3)
      return -1; // manufacturer data + KEYS
  } else if (sector > 0 && sector < 16) {
    if (block == 3) {
      return -1; // block reserved for KEYS
    } else {
      numBlock = sector*4 + block -1;
      return numBlock;
    }
  } else {
    return -2; // invalid sector
  }
}

int clearBlock(int block) {
  uint8_t data[16];
  const uint8_t clearBlock[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  memcpy(data,clearBlock, sizeof data);
  
  return nfc.mifareclassic_WriteDataBlock (block, data);
}

int writeString(int block, String str) {
  uint8_t data[16];
  const uint8_t dataArray[16] = "";
  str.toCharArray(dataArray, 16);
  memcpy(data,dataArray, sizeof data);
  
  return nfc.mifareclassic_WriteDataBlock (block, data);
}

void debugMifareA(uint8_t uid[], int uidLength) {
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  int success;
  for (int i=0; i<16; i++) {
      Serial.print("Sector ");Serial.println(i);
      for (int j=0; j<4; j++) {
        uint8_t data[16];
        success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, i*4+j, 0, keya);
        success = nfc.mifareclassic_ReadDataBlock(i*4+j, data);
        nfc.PrintHexChar(data, 16);
      }
  }
  delay(1000);
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  int valid_uids[] = {4294936564};
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    uint32_t id;
    id = uid[0] | uid[1] << 8 | uid[2] << 16 | uid[3] << 24;

    for (int i = 0; i < sizeof(valid_uids); i++) {
      if (valid_uids[i] == id) {
        Serial.print("YEAAH");
      }
      break;
    }
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
	  
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	  
	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing

      int block = getBlockNumber(2, 2);
      //success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keya);
	  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
		
        // If you want to write something to block 4 to test with, uncomment
		// the following line and this text should be read back in a minute
       // memcpy(data, (const uint8_t[]){'a', 0,0,0,0,0,0,0,0,0,0,0,0,0,0}, sizeof data);
        //clearBlock(getBlockNumber(2,2));
        //writeString(getBlockNumber(2,2), "hola");
        //success = nfc.mifareclassic_WriteDataBlock (block, data);
        debugMifareA(uid, uidLength);
        // Try to read the contents of block 4
        //success = nfc.mifareclassic_ReadDataBlock(block, data);
        success = 0;
        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
		  
          // Wait a bit before reading the card again
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    
    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
	  
      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");
		
        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }
}

