// RHSPIDriver.cpp
//
// Copyright (C) 2014 Mike McCauley
// $Id: RHSPIDriver.cpp,v 1.10 2015/12/16 04:55:33 mikem Exp $

#include <RHSPIDriver.h>

SPIClass SPI2 (&sercom1, FRM95_MISO, FRM95_SCK, FRM95_MOSI, SPI_PAD_2_SCK_3, SERCOM_RX_PAD_0);

RHSPIDriver::RHSPIDriver(uint8_t slaveSelectPin, RHGenericSPI& spi)
    : 
    SPI2(spi),
    _slaveSelectPin(slaveSelectPin)
{
}

bool RHSPIDriver::init()
{
    // start the SPI library with the default speeds etc:
    // On Arduino Due this defaults to SPI1 on the central group of 6 SPI pins
    SPI2.begin();

    // Initialise the slave select pin
    // On Maple, this must be _after_ spi.begin
    pinMode(_slaveSelectPin, OUTPUT);
    digitalWrite(_slaveSelectPin, HIGH);

	pinPeripheral(FRM95_MOSI, PIO_SERCOM);
	pinPeripheral(FRM95_SCK, PIO_SERCOM);
	pinPeripheral(FRM95_MISO, PIO_SERCOM);
	
    delay(100);
    return true;
}

uint8_t RHSPIDriver::spiRead(uint8_t reg)
{
    uint8_t val;
    ATOMIC_BLOCK_START;
    digitalWrite(_slaveSelectPin, LOW);
	SPI2.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    SPI2.transfer(reg & ~RH_SPI_WRITE_MASK); // Send the address with the write mask off
    val = SPI2.transfer(0); // The written value is ignored, reg value is read
	SPI2.endTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    digitalWrite(_slaveSelectPin, HIGH);
    ATOMIC_BLOCK_END;
    return val;
}

uint8_t RHSPIDriver::spiWrite(uint8_t reg, uint8_t val)
{
    uint8_t status = 0;
    ATOMIC_BLOCK_START;
    digitalWrite(_slaveSelectPin, LOW);
    status = SPI2.transfer(reg | RH_SPI_WRITE_MASK); // Send the address with the write mask on
    SPI2.transfer(val); // New value follows
    digitalWrite(_slaveSelectPin, HIGH);
    ATOMIC_BLOCK_END;
    return status;
}

uint8_t RHSPIDriver::spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len)
{
    uint8_t status = 0;
    ATOMIC_BLOCK_START;
    digitalWrite(_slaveSelectPin, LOW);
    status = SPI2.transfer(reg & ~RH_SPI_WRITE_MASK); // Send the start address with the write mask off
    while (len--)
	*dest++ = SPI2.transfer(0);
    digitalWrite(_slaveSelectPin, HIGH);
    ATOMIC_BLOCK_END;
    return status;
}

uint8_t RHSPIDriver::spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len)
{
    uint8_t status = 0;
    ATOMIC_BLOCK_START;
    digitalWrite(_slaveSelectPin, LOW);
    status = SPI2.transfer(reg | RH_SPI_WRITE_MASK); // Send the start address with the write mask on
    while (len--)
	SPI2.transfer(*src++);
    digitalWrite(_slaveSelectPin, HIGH);
    ATOMIC_BLOCK_END;
    return status;
}

void RHSPIDriver::setSlaveSelectPin(uint8_t slaveSelectPin)
{
    _slaveSelectPin = slaveSelectPin;
}
