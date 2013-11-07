/*
Copyright 2013 GHI Electronics LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "FEZLynxS4.h"

using namespace GHI;
using namespace GHI::Interfaces;
using namespace GHI::Mainboards;

#define I2C_DELAY() ;

FEZLynxS4::SoftwareI2CBus::SoftwareI2CBus(CPUPin sda, CPUPin scl) : Interfaces::I2CBus(sda, scl)
{
	this->start = false;
	this->readSCL();
	this->readSDA();
}

FEZLynxS4::SoftwareI2CBus::~SoftwareI2CBus()
{
}

void FEZLynxS4::SoftwareI2CBus::clearSCL() {
	mainboard->setIOMode(this->scl, IOStates::DIGITAL_OUTPUT);
	mainboard->writeDigital(this->scl, false);
}

bool FEZLynxS4::SoftwareI2CBus::readSCL() {
	mainboard->setIOMode(this->scl, IOStates::DIGITAL_INPUT, ResistorModes::PULL_UP);
	return mainboard->readDigital(this->scl);
}

void FEZLynxS4::SoftwareI2CBus::clearSDA() {
	mainboard->setIOMode(this->sda, IOStates::DIGITAL_OUTPUT);
	mainboard->writeDigital(this->sda, false);
}

bool FEZLynxS4::SoftwareI2CBus::readSDA() {
	mainboard->setIOMode(this->sda, IOStates::DIGITAL_INPUT, ResistorModes::PULL_UP);
	return mainboard->readDigital(this->sda);
}

bool FEZLynxS4::SoftwareI2CBus::writeBit(bool bit) {
    if (bit)
		this->readSDA();
    else
		this->clearSDA();

    I2C_DELAY();

	unsigned long endTime = System::TimeElapsed() + 5000;
	while (!this->readSCL() && System::TimeElapsed() < endTime)
		;

    if (bit && !this->readSDA())
		return false;

    I2C_DELAY();
	this->clearSCL();

    return true;
}

bool FEZLynxS4::SoftwareI2CBus::readBit() {
    this->readSDA();

    I2C_DELAY();

	unsigned long endTime = System::TimeElapsed() + 5000;
	while (!this->readSCL() && System::TimeElapsed() < endTime)
		;

    bool bit = this->readSDA();

    I2C_DELAY();
	this->clearSCL();

    return bit;
}

bool FEZLynxS4::SoftwareI2CBus::sendStartCondition() {
	if (this->start) {
		this->readSDA();
		I2C_DELAY();

		unsigned long endTime = System::TimeElapsed() + 5000;
		while (!this->readSCL() && System::TimeElapsed() < endTime)
			;

	}

    if (!this->readSDA())
		return false;

	this->clearSDA();
	I2C_DELAY();
	this->clearSCL();

	this->start = true;

	return true;
}

bool FEZLynxS4::SoftwareI2CBus::sendStopCondition() {
	this->clearSDA();
	I2C_DELAY();

	unsigned long endTime = System::TimeElapsed() + 5000;
	while (!this->readSCL() && System::TimeElapsed() < endTime)
		;

	if (!this->readSDA())
		return false;

	I2C_DELAY();
	this->start = false;

	return true;
}

bool FEZLynxS4::SoftwareI2CBus::transmit(bool sendStart, bool sendStop, unsigned char data) {
	unsigned char bit;
	bool nack;

	if (sendStart)
		this->sendStartCondition();

    for (bit = 0; bit < 8; bit++) {
		this->writeBit((data & 0x80) != 0);

		data <<= 1;
    }

    nack = this->readBit();

	if (sendStop)
		this->sendStopCondition();

     return nack;
}

unsigned char FEZLynxS4::SoftwareI2CBus::receive(bool sendAcknowledgeBit, bool sendStopCondition) {
	unsigned char d = 0;
	unsigned char bit = 0;

	for (bit = 0; bit < 8; bit++) {
		d <<= 1;

		if (this->readBit())
			d |= 1;
	}

	this->writeBit(!sendAcknowledgeBit);

	if (sendStopCondition)
		this->sendStopCondition();

	return d;
}

unsigned int FEZLynxS4::SoftwareI2CBus::write(const unsigned char* buffer, unsigned int count, unsigned char address, bool sendStop)
{
	if (!count)
		return 0;

	unsigned int numWrite = 0;
	unsigned int i = 0;

	if (!this->transmit(true, false, address))
		for (i = 0; i < count - 1; i++)
			if (!this->transmit(false, false, buffer[i]))
				numWrite++;

	if (!this->transmit(false, sendStop, buffer[i]))
		numWrite++;

	return numWrite;
 }

unsigned int FEZLynxS4::SoftwareI2CBus::read(unsigned char* buffer, unsigned int count, unsigned char address, bool sendStop)
{
	if (!count)
		return 0;

	unsigned int numRead = 0;
	unsigned int i = 0;

	if (!this->transmit(true, false, address | 1)) {
		for (i = 0; i < count - 1; i++) {
			buffer[i] = this->receive(true, false);
			numRead++;
		}
	}

    buffer[i] = this->receive(false, sendStop);
	numRead++;

    return numRead;
}

bool FEZLynxS4::SoftwareI2CBus::writeRead(const unsigned char* writeBuffer, unsigned int writeLength, unsigned char* readBuffer, unsigned int readLength, unsigned int* numWritten, unsigned int* numRead, unsigned char address)
{
	*numWritten = 0;
	*numRead = 0;

	unsigned int i = 0;
	unsigned int write = 0;
	unsigned int read = 0;

    if (writeLength > 0) {
		if (!this->transmit(true, false, address)) {
			for (i = 0; i < writeLength - 1; i++) {
				if (!this->transmit(false, false, writeBuffer[i])) {
					(write)++;
				}
			}
		}

		if (!this->transmit(false, (readLength == 0), writeBuffer[i]))
			write++;

		*numWritten = write;
    }

    if (readLength > 0) {
		if (!this->transmit(true, false, address | 1)) {
			for (i = 0; i < readLength - 1; i++) {
				readBuffer[i] = this->receive(true, false);
				read++;
			}
		}

		readBuffer[i] = this->receive(false, true);
		read++;
		*numRead = read;
    }

	return (write + read) == (writeLength + readLength);
}
