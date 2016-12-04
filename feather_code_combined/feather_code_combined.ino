    // Feather9x_TX
    // -*- mode: C++ -*-
    // Example sketch showing how to create a simple messaging client (transmitter)
    // with the RH_RF69 class. RH_RF69 class does not provide for addressing or
    // reliability, so you should only use RH_RF69 if you do not need the higher
    // level messaging abilities.
    // It is designed to work with the other example Feather9x_RX
     
    #include <SPI.h>
    #include <RH_RF69.h>

    #include <Arduino.h>
    #if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
      #include <SoftwareSerial.h>
    #endif
    
    #include "Adafruit_BLE.h"
    #include "Adafruit_BluefruitLE_SPI.h"
    #include "Adafruit_BluefruitLE_UART.h"
    #include "BluefruitConfig.h"


    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
     
    // for feather m0  
    #define RFM69_CS A1
    #define RFM69_RST 6
    #define RFM69_INT A2
    #define RFM69_MOSI 10
    #define RFM69_MISO 11
    #define RFM69_SCK 12    
     
    #define RF69_FREQ 915.0
     
    // Singleton instance of the radio driver
    RH_RF69 rf69(RFM69_CS, RFM69_INT);
    uint8_t data = 0;
    
    // A small helper
    void error(const __FlashStringHelper*err) {
      Serial.println(err);
      while (1);
    }

    
    void setup() 
    {
      pinMode(RFM69_RST, OUTPUT);
      digitalWrite(RFM69_RST, HIGH);
     
      //while (!Serial);
      Serial.begin(9600);
      delay(100);
     
      Serial.println("Feather LoRa TX Test!");
     
      // manual reset
      digitalWrite(RFM69_RST, LOW);
      delay(10);
      digitalWrite(RFM69_RST, HIGH);
      delay(10);
      Serial.println("Reset complete");
      
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


      
      if ( !ble.begin(VERBOSE_MODE) )
      {
        error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
      }
      Serial.println( F("OK!") );
    
      if ( FACTORYRESET_ENABLE )
      {
        /* Perform a factory reset to make sure everything is in a known state */
        Serial.println(F("Performing a factory reset: "));
        if ( ! ble.factoryReset() ){
          error(F("Couldn't factory reset"));
        }
      }
    
      /* Disable command echo from Bluefruit */
      ble.echo(false);
    
      Serial.println("Requesting Bluefruit info:");
      /* Print Bluefruit information */
      ble.info();
    
      Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
      Serial.println(F("Then Enter characters to send to Bluefruit"));
      Serial.println();
    
      ble.verbose(false);  // debug info is a little annoying after this point!
      while (! ble.isConnected()) {
        delay(500);
      }

      // LED Activity command is only supported from 0.6.6
      if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
      {
        // Change Mode LED Activity
        Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
        ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
      }

      // Set module to DATA mode
      Serial.println( F("Switching to DATA mode!") );
      ble.setMode(BLUEFRUIT_MODE_DATA);
      
      ble.print("Bluetooth initialized");
      Serial.println("Bluetooth initialized");
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
          buf[len] = 0;
          ble.print((char*)buf);
              
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
