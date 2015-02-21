// Adapted from Jeff Rowberg's excellent I2C library.
// His original code may be found at https://github.com/sparkfun/MPU-9150_Breakout/tree/master/firmware
// Licensing information may be found inside this repository's ReadMe file.
// This is essentially an interface to WPILib's implementation of I2C so that I don't have to rewrite the entirety of the MPU-9150 firmware
// Any functions dependant upon writeWord(s) must be removed from MPU-9150.cpp

#pragma once

#include "WPILib.h"


#define MPU9150_I2C_ADR_LOW 0x68
#define MPU9150_I2C_ADR_HIGH 0x69
#define MPU9150_I2C_ADR MPU9150_I2C_ADR_LOW


class I2Cdev {
	public:
		I2Cdev(uint8_t _devAddr = MPU9150_I2C_ADR) {
			reader = new I2C(devAddr);
		}


		// readBit: readByte
		static int8_t readBit(uint8_t _devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);

		// readBits: readByte
		static int8_t readBits(uint8_t _devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);

		// readByte: readBytes
		static int8_t readByte(uint8_t _devAddr, uint8_t regAddr, uint8_t *data);        

		// readBytes: self
		static int8_t readBytes(uint8_t _devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
			// bool Read(uint8_t registerAddress, uint8_t count, uint8_t *data);
			return static_cast<int8_t>(Read(regAddr, length, data));
		}
		

		// writeBit: readByte, writeByte
		static bool writeBit(uint8_t _devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);

		// writeBits: readByte, writeByte
		static bool writeBits(uint8_t _devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);

		// writeByte: writeBytes
		static bool writeByte(uint8_t _devAddr, uint8_t regAddr, uint8_t data);

		// writeBytes: self
		static bool writeBytes(uint8_t _devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
			// bool Write(uint8_t registerAddress, uint8_t data);
			return WriteBulk(regAddr, *data)
		}


		static uint16_t readTimeout = 1000;

	private:
		static uint8_t devAddr;
		static I2C* reader;
};
