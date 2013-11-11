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

#include "SPIBus.h"

using namespace GHI;
using namespace GHI::Interfaces;

SPIBus::SPIBus(CPUPin mosiPin, CPUPin misoPin, CPUPin sckPin) : mosi(mosiPin), miso(misoPin), sck(sckPin) 
{	

}

SPIBus::~SPIBus() 
{
	for (SPIDevice* current = (SPIDevice*)this->spiDevices.start(); !this->spiDevices.ended(); current = (SPIDevice*)this->spiDevices.next())
		delete current;
}

SPIDevice* SPIBus::getSPIDevice(CPUPin pin, SPIConfiguration* configuration) 
{
	SPIDevice* device = new SPIDevice(this, pin, configuration);
	this->spiDevices.addV(device);
	return device; 
}

void SPIBus::writeRead(const unsigned char* sendBuffer, unsigned char* receiveBuffer, unsigned int count, SPIConfiguration* configuration, bool deselectAfter) 
{
	mainboard->panic(Exceptions::ERR_SPI_NOT_SUPPORTED);
}