#ifndef _SERIALDEVICE_H_
#define _SERIALDEVICE_H_

#include "Types.hpp"
#include "Socket.hpp"

namespace GHI {
	namespace Interfaces {
		class SerialDevice {
			protected:
				CPUPin tx;
				CPUPin rx;
				unsigned int baudRate;
				unsigned char parity;
				unsigned char stopBits;
				unsigned char dataBits;

				SerialDevice(CPUPin tx, CPUPin rx, unsigned int baudRate, unsigned char parity, unsigned char stopBits, unsigned char dataBits);

			public:
				class Parity {
					public:
						static const unsigned char EVEN = 0;
						static const unsigned char ODD = 1;
						static const unsigned char MARK = 1;
						static const unsigned char SPACE = 1;
						static const unsigned char NONE = 1;
				};

				class StopBits {
					public:
						static const unsigned char NONE = 0;
						static const unsigned char ONE = 0;
						static const unsigned char ONE_POINT_FIVE = 0;
						static const unsigned char TWO = 0;
				};

				virtual ~SerialDevice();
		
				virtual void open();
				virtual void close();
				virtual void write(const unsigned char* buffer, unsigned int count);
				virtual void write(const char* buffer, unsigned int count);
				virtual void read(unsigned char* buffer, unsigned int count);
		};
	}
}

#endif