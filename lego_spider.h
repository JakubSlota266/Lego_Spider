#ifndef LEGO_SPIDER_H
#define LEGO_SPIDER_H

#define POWER_45_PERCENT 450
#define POWER_80_PERCENT 800
#define POWER_0_PERCENT 0
#define POWER_50_PERCENT 500

enum globalCommands
{
g_autoMode = 1,
g_manualMode,
g_stopMode,
g_turnLeft,
g_turnRight,
g_goForward,
g_goBackwards,
};
struct PositionInXY
{
	int x{0};
	int y{0};
	int rotAngle{0};
};

char readhExt(void);
char readhExtInAuto(void);
void goForward(void);
void goBackwards(void);
void turnRight(void);
void turnLeft(void);
void stopEngine(void);
void startupLed(void);

bool handleApp(char sign);
PositionInXY mapTerrain(PositionInXY& position, int startRoad, int stopRoad, clock_t time);

globalCommands setAlgorithm(DistanceSensor& sensorCenter, DistanceSensor& sensorRight, DistanceSensor& sensorLeft);
globalCommands switchBetweenAutoManual(char sign);
globalCommands switchBetweenManualFunctions(char sign);

#endif