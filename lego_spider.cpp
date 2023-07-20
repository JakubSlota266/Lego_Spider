#include "include\\hFramework.h"
#include <stddef.h>
#include <stdio.h>
#include "lego_spider.h"
#include "DistanceSensor.h"
#include <chrono>
#include <array>
#define MINIMAL_DISTANCE_TO_ROTATE 30

using namespace hFramework;
using namespace hModules;

PositionInXY positionInXY{};
std::array<PositionInXY, 1000> arrayStoringPosition{};

enum dirOfRotation
{
	rotLeft = 1,
	rotRight,
};

char readhExt()
{
while(true)
{
if(hExt.serial.available() != (-1))
{
	char received_data[1];
	if (hExt.serial.read(received_data, 1, 10000))
		{
			printf("hExt3.serial has received data: %s\r\n", received_data);
			return received_data[0];
		}
	}
}
}
void goForward()
{
	hMot1.setPower(POWER_80_PERCENT); 
	hMot4.setPower(-POWER_80_PERCENT); 
}

void goBackwards()
{
	hMot1.setPower(-POWER_80_PERCENT); 
	hMot4.setPower(POWER_80_PERCENT); 
}

void turnRight()
{
	hMot1.setPower(POWER_45_PERCENT); 
	hMot4.setPower(POWER_45_PERCENT); 
}

void turnLeft()
{
	hMot1.setPower(-POWER_45_PERCENT); 
	hMot4.setPower(-POWER_45_PERCENT); 
}

void stopEngine()
{
	hMot1.setPower(POWER_0_PERCENT);
	hMot4.setPower(POWER_0_PERCENT);
}

globalCommands setAlgorithm(DistanceSensor& sensorCenter, DistanceSensor& sensorRight, DistanceSensor& sensorLeft)
{
	
	int startPoint{0};
	int stopPoint{0};

	//==== HANDLE MAPPING ====//
	bool stoppedAfterRotation{true};
	bool robotFirstTurn{true};
	
	int index{0};
	double timeOfTheRotation{0};

	while(true)
	{
		char quitAuto = readhExtInAuto();

		switch(quitAuto)
		{
			case 'Z':
			{
				return g_stopMode;
			}

			case 'E':
			{
			int distanceCenter = sensorCenter.getDistance();
			int distanceRight = sensorRight.getDistance();
			int distanceLeft = sensorLeft.getDistance();
			
			//timeOfTheRotation = ((double) (start - stop)) / CLOCKS_PER_SEC;
		
			Serial.printf("Sensor Center: %d\r\n", distanceCenter);
			Serial.printf("Sensor right: %d\r\n", distanceRight);
			Serial.printf("Sensor left: %d\r\n", distanceLeft);

			dirOfRotation dirOfRot;

			if (distanceLeft > distanceRight)
			{
				dirOfRot = rotLeft;
			}
			else
			{
				dirOfRot = rotRight;
			}
			
			if(startPoint > 0 && stopPoint > 0)
			{
				arrayStoringPosition[index++] = mapTerrain(positionInXY, startPoint, stopPoint, timeOfTheRotation);
			}

			if(distanceCenter > MINIMAL_DISTANCE_TO_ROTATE && distanceRight > MINIMAL_DISTANCE_TO_ROTATE && distanceLeft > MINIMAL_DISTANCE_TO_ROTATE)
			{
				if(stoppedAfterRotation)
				{
					startPoint = distanceCenter;
				}
				stoppedAfterRotation = false;

				if(!robotFirstTurn)
				{
					auto start = std::chrono::high_resolution_clock::now();
					robotFirstTurn = true;
				}

				goForward();
				
			}
			else if (distanceCenter == -1 || distanceRight == -1 || distanceLeft == -1)
			{
					break; 
			}
			else if (distanceRight < MINIMAL_DISTANCE_TO_ROTATE || (distanceCenter < MINIMAL_DISTANCE_TO_ROTATE && dirOfRot == rotRight))
			{
				
				if(!stoppedAfterRotation)
				{
					stopPoint = distanceCenter;
				}
				stoppedAfterRotation = true;

				if(robotFirstTurn)
				
				{
					auto stop = std::chrono::high_resolution_clock::now();
					robotFirstTurn = false;
				}

				turnLeft();
			}
			else if(distanceLeft < MINIMAL_DISTANCE_TO_ROTATE || (MINIMAL_DISTANCE_TO_ROTATE < 30 && dirOfRot == rotLeft))
			{
				turnRight();
			}
			}
			default: break;
		}
	}
}

bool handleApp(char sign)
{
	while(true)
	{
	switch(sign)
	{
		case 'U':
		{
			hLED1.on();
			char autoManualSwitch = readhExt();
			globalCommands chosenMod = switchBetweenAutoManual(autoManualSwitch);

		//======= AUTO CASE ======//
		if(chosenMod == g_autoMode)
		{
			hLED2.on();
			printf("Chose automatic mode\n");

			DistanceSensor sensorRight(hSens1);
			DistanceSensor sensorCenter(hSens5);
			DistanceSensor sensorLeft(hSens3);
			
			if(setAlgorithm(sensorCenter, sensorRight, sensorLeft) == g_stopMode)
			goto stopcase;

			return true;
		}
		//======= MANUAL CASE ======//
		else if(chosenMod == g_manualMode)		
		{
			hLED3.on();
			printf("Chose manual mode\n");
			while(true)
			{
				char nextCommandForManual = readhExt();
				
				if(switchBetweenManualFunctions(nextCommandForManual) == g_stopMode)
				{
					goto stopcase;
				}
				else if (switchBetweenManualFunctions(nextCommandForManual) == g_goForward)
					goForward();
				else if (switchBetweenManualFunctions(nextCommandForManual) == g_goBackwards)
					goBackwards();
				else if (switchBetweenManualFunctions(nextCommandForManual) == g_turnLeft)
					turnLeft();
				else if (switchBetweenManualFunctions(nextCommandForManual) == g_turnRight)
					turnRight();
			}
		}
		else if(chosenMod == g_stopMode)
		{
			goto stopcase;
		}
		}

		//======= STOP CASE ======//
		case 'Z':
		{
		stopcase:
		hLED1.off(); hLED2.off(); hLED3.off();
		printf("Stop mode\n");	
		stopEngine();
		return false;			
		}
		//Should never happen
		default: break;
	}
	}
}

globalCommands switchBetweenAutoManual(char sign)
{
	switch(sign)
	{
		case 'A':
		{
		return g_autoMode;
		}
		case 'M':
		{
		return g_manualMode;
		}
		case 'Z':
		{
			return g_stopMode;
		}
	
		default: return g_stopMode;
	}
		
}

globalCommands switchBetweenManualFunctions(char sign)
{
	switch(sign)
	{
		case 'F':
		return g_goForward;
		case 'B':
		return g_goBackwards;
		case 'R':
		return g_turnRight;
		case 'L':
		return g_turnLeft;
		case 'Z':
		return g_stopMode;

		default: return g_stopMode;
	}
}

PositionInXY mapTerrain(PositionInXY& position, int startRoad, int stopRoad, clock_t time)
{
	sys.delay(100);
	int road = abs(startRoad - stopRoad);
	position.rotAngle = coeffitient * time;
	position.x += position.x + sin(position.rotAngle) * road;
	position.y += position.y + cos(position.rotAngle) * road;
	
	return position;
}
void startupLed()
{
	hLED1.off();
	hLED2.off();
	hLED3.off();
}