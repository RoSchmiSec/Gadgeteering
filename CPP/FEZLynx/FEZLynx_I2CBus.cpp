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

#include "../Gadgeteering/Gadgeteering.h"

#include "FEZLynx.h"

using namespace GHI;
using namespace GHI::Interfaces;
using namespace GHI::Mainboards;

#define I2C_DELAY() ;

FEZLynx::I2CBus::I2CBus(CPUPin sda, CPUPin scl) : Interfaces::I2CBus(sda, scl)
{
    this->start = false;
    this->readSCL();
    this->readSDA();
}

FEZLynx::I2CBus::~I2CBus()
{

}

void FEZLynx::I2CBus::clearSCL()
{
    mainboard->setIOMode(this->scl, IOStates::DIGITAL_OUTPUT);
    mainboard->writeDigital(this->scl, false);
}

bool FEZLynx::I2CBus::readSCL()
{
    mainboard->setIOMode(this->scl, IOStates::DIGITAL_INPUT, ResistorModes::PULL_UP);
    return mainboard->readDigital(this->scl);
}

void FEZLynx::I2CBus::clearSDA()
{
    mainboard->setIOMode(this->sda, IOStates::DIGITAL_OUTPUT);
    mainboard->writeDigital(this->sda, false);
}

bool FEZLynx::I2CBus::readSDA()
{
    mainboard->setIOMode(this->sda, IOStates::DIGITAL_INPUT, ResistorModes::PULL_UP);
    return mainboard->readDigital(this->sda);
}

bool FEZLynx::I2CBus::writeBit(bool bit)
{
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

bool FEZLynx::I2CBus::readBit()
{
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

bool FEZLynx::I2CBus::sendStartCondition()
{
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

bool FEZLynx::I2CBus::sendStopCondition()
{
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

bool FEZLynx::I2CBus::transmit(bool sendStart, bool sendStop, unsigned char data) {
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

unsigned char FEZLynx::I2CBus::receive(bool sendAcknowledgeBit, bool sendStopCondition)
{
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

unsigned int FEZLynx::I2CBus::write(const unsigned char* buffer, unsigned int count, unsigned char address, bool sendStop)
{
    char activeState = 0x10;
    char *OutputBuffer = new char[count +3];

//    if(configuration->chipSelectActiveState && configuration->clockEdge)
//        activeState = 0x10;

    DWORD dwNumBytesToSend = 0; //Clear output buffer
    DWORD dwNumBytesSent = 0;

    OutputBuffer[dwNumBytesToSend++] = 0x31;//0x31 ; //Clock data byte out on +ve Clock Edge LSB first
    OutputBuffer[dwNumBytesToSend++] = 0;
    OutputBuffer[dwNumBytesToSend++] = count - 1; //Data length of 0x0000 means 1 byte data to clock out

    for(int i = 0; i < count; i++)
        OutputBuffer[dwNumBytesToSend++] = (char)buffer[i];

    FT_STATUS ftStatus = FT_Write(channel, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent); //Send off the commands

    if(ftStatus != FT_OK)
        mainboard->panic(0x35);

    return dwNumBytesSent;
 }

unsigned int FEZLynx::I2CBus::read(unsigned char* buffer, unsigned int count, unsigned char address, bool sendStop)
{
    DWORD dwBytesInQueue = 0;
    int timeout = 0;
    FT_STATUS ftStatus = FT_OK;

    //wait for queue to fill to desired amount, or timeout
    while((dwBytesInQueue < count) && (timeout < 500))
    {
        ftStatus |= FT_GetQueueStatus(channel, &dwBytesInQueue);
        System::Sleep(1);
        timeout++;
    }

    if((timeout >= 499) || (ftStatus != FT_OK))
        mainboard->panic(0x35);

    DWORD dwNumBytesRead = 0;
    ftStatus = FT_Read(channel, buffer, count, &dwNumBytesRead);

    if((ftStatus != FT_OK))
        mainboard->panic(0x35);

    return dwNumBytesRead;
}

bool FEZLynx::I2CBus::writeRead(const unsigned char* writeBuffer, unsigned int writeLength, unsigned char* readBuffer, unsigned int readLength, unsigned int* numWritten, unsigned int* numRead, unsigned char address)
{
    *numWritten = 0;
    *numRead = 0;

    unsigned int i = 0;
    unsigned int write = 0;
    unsigned int read = 0;

    write = *numWritten = this->write(writeBuffer, writeLength, address, false);
    read = *numRead = this->read(readBuffer, readLength, address, true);

    return (write + read) == (writeLength + readLength);
}

void FEZLynx::I2CBus::SetChannel(FT_HANDLE i2cChannel)
{
    this->channel = i2cChannel;
}