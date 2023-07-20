#include "include\hFramework.h"
#include <stddef.h>
#include <stdio.h>
#include "DistanceSensor.h"
#include <chrono>
#include <array>

#include "lego_spider.h"
//using namespace hModules; 
using namespace hFramework;
using namespace hModules;
char received_data[1];

void hMain()
{
	// Handle Bluetooth Connection
	sys.setSysLogDev(&devNull);		//turn off system logs on Serial
	sys.setLogDev(&Serial);									
	hExt.serial.init(9600, Parity::None, StopBits::One);   
	startupLed();
	char nextGeneralCommand;
	for (;;)
	{
		nextGeneralCommand = readhExt();
		handleApp(nextGeneralCommand);
		
	}
}