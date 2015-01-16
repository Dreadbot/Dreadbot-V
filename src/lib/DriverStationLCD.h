/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __DRIVER_STATION_LCD_H__
#define __DRIVER_STATION_LCD_H__

#include "SensorBase.h"

/**
 * Provide access to LCD on the Driver Station.
 * 
 * Buffer the printed data locally and then send it
 * when UpdateLCD is called.
 */
class DriverStationLCD : public SensorBase
{
public:
	static const UINT32 kSyncTimeout_ms = 20;
	static const UINT16 kFullDisplayTextCommand = 0x9FFF;
	static const INT32 kLineLength = 21;
	enum Line {kMain_Line6, kUser_Line2, kUser_Line3, kUser_Line4, kUser_Line5, kUser_Line6};

	virtual ~DriverStationLCD();
	static DriverStationLCD *GetInstance();

	void UpdateLCD();
	void Printf(Line line, UINT32 startingColumn, const char *writeFmt, ...);

protected:
	DriverStationLCD();

private:
	static void InitTask(DriverStationLCD *ds);
	static DriverStationLCD *m_instance;
	DISALLOW_COPY_AND_ASSIGN(DriverStationLCD);

	char *m_textBuffer;
	SEM_ID m_textBufferSemaphore;
};

#endif

