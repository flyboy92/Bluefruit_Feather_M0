    // Feather9x_RX
    // -*- mode: C++ -*-
    // Example sketch showing how to create a simple messaging client (receiver)
    // with the RH_RF69 class. RH_RF69 class does not provide for addressing or
    // reliability, so you should only use RH_RF69 if you do not need the higher
    // level messaging abilities.
    // It is designed to work with the other example Feather9x_TX
     
    #include <SPI.h>
    #include <RH_RF69.h>
     
    // for feather m0  
    #define RFM69_CS A1
    #define RFM69_RST 6
    #define RFM69_INT A2
    #define RFM69_MOSI 10
    #define RFM69_MISO 11
    #define RFM69_SCK 12

    // for feather m0 bluefruit
    #define BLUEFRUIT_SPI_CS               8
    #define BLUEFRUIT_SPI_IRQ              7
    #define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused
     
    // Change to 434.0 or other frequency, must match RX's freq!
    #define RF69_FREQ 915.0
     
    // Singleton instance of the radio driver
    RH_RF69 rf69(RFM69_CS, RFM69_INT);
     
    // Blinky on receipt
    #define LED 13
     
    void setup() 
    {
      pinMode(LED, OUTPUT);     
      pinMode(RFM69_RST, OUTPUT);
      digitalWrite(RFM69_RST, HIGH);
     
      while (!Serial);
      Serial.begin(9600);
      delay(100);
     
      Serial.println("Feather LoRa RX Test!");
      
      // manual reset
      digitalWrite(RFM69_RST, LOW);
      delay(10);
      digitalWrite(RFM69_RST, HIGH);
      delay(10);
     
      while (!rf69.init()) {
        Serial.println("LoRa radio init failed");
        while (1);
      }
      Serial.println("LoRa radio init OK!");
     
      // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
      if (!rf69.setFrequency(RF69_FREQ)) {
        Serial.println("setFrequency failed");
        while (1);
      }
      Serial.print("Set Freq to: "); Serial.println(RF69_FREQ);
     
      // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
     
      // The default transmitter power is 13dBm, using PA_BOOST.
      // If you are using RFM69/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
      // you can set transmitter powers from 5 to 23 dBm:
      rf69.setTxPower(23);
    }
     
    void loop()
    {
      if (rf69.available())
      {
        // Should be a message for us now   
        uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        
        if (rf69.recv(buf, &len))
        {
          digitalWrite(LED, HIGH);
          RH_RF69::printBuffer("Received: ", buf, len);
          Serial.print("Got: ");
          Serial.println((char*)buf);
           Serial.print("RSSI: ");
          Serial.println(rf69.lastRssi(), DEC);
          
          // Send a reply
          uint8_t data[] = "And hello back to you";
          rf69.send(data, sizeof(data));
          rf69.waitPacketSent();
          Serial.println("Sent a reply");
          digitalWrite(LED, LOW);
        }
        else
        {
          Serial.println("Receive failed");
        }
      }
    }
