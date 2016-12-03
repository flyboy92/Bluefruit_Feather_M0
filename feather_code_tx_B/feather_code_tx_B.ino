    // Feather9x_TX
    // -*- mode: C++ -*-
    // Example sketch showing how to create a simple messaging client (transmitter)
    // with the RH_RF69 class. RH_RF69 class does not provide for addressing or
    // reliability, so you should only use RH_RF69 if you do not need the higher
    // level messaging abilities.
    // It is designed to work with the other example Feather9x_RX
     
    //#include <SPI.h>
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
    
     
    #define RF69_FREQ 915.0
     
    // Singleton instance of the radio driver
    RH_RF69 rf69(RFM69_CS, RFM69_INT);
    uint8_t data = 0;
    void setup() 
    {
      pinMode(RFM69_RST, OUTPUT);
      digitalWrite(RFM69_RST, HIGH);
     
      while (!Serial);
      Serial.begin(9600);
      delay(100);
     
      Serial.println("Feather LoRa TX Test!");
     
      // manual reset
      digitalWrite(RFM69_RST, LOW);
      delay(10);
      digitalWrite(RFM69_RST, HIGH);
      delay(10);

      Serial.println("Reset complete");

      /*
      rf69.init();
      digitalWrite(RFM69_CS, LOW);
      SPI2.transfer(0x04);
      data = SPI2.transfer(0x00);
      digitalWrite(RFM69_CS, HIGH);

      Serial.print("ADDR 0x04: ");
      Serial.println(data);
      delay(100);
      */
      
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
      // rf69.setTxPower(23);
    }
     
    int16_t packetnum = 0;  // packet counter, we increment per xmission
     
    void loop()
    {
      Serial.println("Sending to rf69_server");
      // Send a message to rf69_server
      
      char radiopacket[20] = "Hello World #      ";
      itoa(packetnum++, radiopacket+13, 10);
      Serial.print("Sending "); Serial.println(radiopacket);
      radiopacket[19] = 0;
      
      Serial.println("Sending..."); delay(10);
      rf69.send((uint8_t *)radiopacket, 20);
     
      Serial.println("Waiting for packet to complete..."); delay(10);
      rf69.waitPacketSent();
      // Now wait for a reply
      uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
     
      Serial.println("Waiting for reply..."); delay(10);
      if (rf69.waitAvailableTimeout(1000))
      { 
        // Should be a reply message for us now   
        if (rf69.recv(buf, &len))
       {
          Serial.print("Got reply: ");
          Serial.println((char*)buf);
          Serial.print("RSSI: ");
          Serial.println(rf69.lastRssi(), DEC);    
        }
        else
        {
          Serial.println("Receive failed");
        }
      }
      else
      {
        Serial.println("No reply, is there a listener around?");
      }
      delay(1000);
    }
