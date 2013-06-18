#include <Gadgeteering.h>
#include <FEZMedusa.h>
#include <IO60P16.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <LED7R.h>

using namespace GHI;
using namespace GHI::Interfaces;
using namespace GHI::Mainboards;
using namespace GHI::Modules;

bool nextOn = true;
FEZMedusa board;
LED7R led(10);
LED7R led1(1);

void setup() {
  
}

void loop() {  
  led.turnAllOff();
  led1.turnAllOff();
  
  delay(1000);
    
  for (int i = 1; i <= 7; i++) {
    
    if (nextOn) {
      led.turnOnLED(i);
      led1.turnOnLED(i);
    }
    else {
      led.turnOffLED(i);
      led1.turnOffLED(i);
    }
    
    delay(250);
  }
}

