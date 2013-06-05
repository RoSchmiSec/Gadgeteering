#ifndef _MAINBOARD_H_
#define _MAINBOARD_H_

#include "Types.hpp"
#include "Socket.hpp"
#include "SPIBus.hpp"
#include "SerialDevice.hpp"

namespace GHI {
	class ExtenderChip;

	class Module {
		protected:
			Module();
	};
	
	class Mainboard {
		struct ListNode {
			void* node;
			ListNode* next;
		};

		ListNode* sockets;

		protected:
			Mainboard();
			virtual ~Mainboard();

			Socket* registerSocket(Socket* socket);

		public:
			void panic(const char* error);
			Socket* getSocket(int number);

			virtual void setPWM(Socket* socket, Socket::Pin pin, double dutyCycle, double frequency);
			virtual bool readDigital(Socket* socket, Socket::Pin pin);
			virtual void writeDigital(Socket* socket, Socket::Pin pin, bool value);
			virtual double readAnalog(Socket* socket, Socket::Pin pin);
			virtual void writeAnalog(Socket* socket, Socket::Pin pin, double voltage);
			virtual void setIOMode(Socket* socket, Socket::Pin pin, IOState state, ResistorMode resistorMode = ResistorModes::FLOATING);
			
			virtual GHI::Interfaces::SPIBus* getNewSPIBus(Socket* socket);
			virtual GHI::Interfaces::SerialDevice* getNewSerialDevice(Socket* socket, int baudRate, int parity, int stopBits, int dataBits);
	};

	extern GHI::Mainboard* mainboard;
}

#endif
