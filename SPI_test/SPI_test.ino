#include <SPI.h>
#include "wiring_private.h" // pinPeripheral() function

#define RFM95_CS A1
#define RFM95_RST 6
#define RFM95_INT A2
#define FRM95_MOSI 10
#define FRM95_MISO 11
#define FRM95_SCK 12

SPIClass mySPI (&sercom1, FRM95_MISO, FRM95_SCK, FRM95_MOSI, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);
 
void setup() {

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  // do this first, for Reasons
  mySPI.begin();

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  // Assign pins 11, 12, 13 to SERCOM functionality
  pinPeripheral(FRM95_MOSI, PIO_SERCOM);
  pinPeripheral(FRM95_SCK, PIO_SERCOM);
  pinPeripheral(FRM95_MISO, PIO_SERCOM);

  pinMode(RFM95_CS, OUTPUT);
  digitalWrite(RFM95_CS, HIGH);
}
 
uint8_t data=0;
void loop() {
  digitalWrite(RFM95_CS, LOW);
  mySPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  mySPI.transfer(0x03);
  data = mySPI.transfer(0x00);
  mySPI.endTransaction();
  digitalWrite(RFM95_CS, HIGH);
  Serial.println(data);
  delay(100);
}
