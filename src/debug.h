/*! \file debug.h
	\brief Debug Functions and Tasks
	
	Provides tasks and functions for debugging purposes. Like
	flashing an LED corresponding to state, printing motorstatus
	and the likes.
	
	\author thomas.zink
	\version 20101123
	
	Changelog:
		- 20110517 thomas.zink
			- corrections on documentation
			- moved all remaining debugging functions to debug.h
			- added debug task for easier debugging
		- 201012xx jens.erat
			- add printInformation for nicer output
			- removed motorstatus
			- added robotstatus
		- 20101123 thomas.zink
			- initial version
			- moved tasks and functions from main file
*/

/*!
	\brief Prints description and a number
	
	Prints a numerical value with its description
	(may be up to 98 characters) to a given line.
	The line is cleared before printing.
	
	\param	y			Line number, can be a constant
	\param	desc	Description to print, string
	\param	val		The integer value to print.
*/
void printInformation(int y, string desc, int val)
{
	ClearLine(y);
	TextOut(0, y, desc);
	NumOut(8*9, y, val);    
}
// printInformation

/*!
	\brief Prints the status of the motors on LCD
	
	This task continuously prints information about the robots
	actuators and sensors on the LCD in a 100 ms interval. 
*/
task robotstatus (void)
{
	while (true) {
		int i;
		printInformation(LCD_LINE1, "speed <-:", MotorActualSpeed(MOTOR_LEFT));
		printInformation(LCD_LINE2, "speed ->:", MotorActualSpeed(MOTOR_RIGHT));
		printInformation(LCD_LINE3, "light:", LIGHT_VALUE);
		printInformation(LCD_LINE4, "color:", COLOR_VALUE);		
		printInformation(LCD_LINE5, "surface:", surface);		
		printInformation(LCD_LINE6, "rot. <-:", abs(MotorRotationCount(MOTOR_LEFT)));
		printInformation(LCD_LINE7, "rot. ->:", abs(MotorRotationCount(MOTOR_RIGHT)));
		printInformation(LCD_LINE8, "state :", abs(state));			
		Wait(100);       
	}
}
// robotstatus

/*!
	\brief Flashes the LED according to state
	
	Depending on the state the robot is in this task flashes the
	RCX LED in different patterns.
*/
task ledflash (void)
{
	while(true) {
		OnFwd(LED,100);
		while (state == STATE_LINE_FOLLOW || state == STATE_LINE_SEARCH) {
			OnFwd(LED,100);
			Wait(200);
			Off(LED);
			Wait(500);
		}
		while (state == STATE_JUNC_PASS || state == STATE_JUNC_LOOK) {
			OnFwd(LED,100);
			Wait(200);
			Off(LED);
			Wait(60);
			OnFwd(LED,100);
			Wait(200);
			Off(LED);
			Wait(500);
		}
		while (state == STATE_EXIT) {
			OnFwd(LED,100);
			Wait(60);
			Off(LED);
			Wait(60);
		}
	}
}
// ledflash

/*!
	\brief Outputs the current state on the HT prototype board
	
	This tasks sends the current state to the digital outputs B
	of the HT prototype board. Depending on the definitions of
	the state the value can be differently interpreted.
	The current solution assumes a D6 circuitry implemented on
	the HT board with 7 LEDs that are linked as on a D6. The
	LEDs will be lit according to state. See the project NXT-D6 for
	building instructions. However, since only the state is sent
	as a byte to the HT board, any desired output circuitry can
	be used.
*/
task showstate (void)
{
	while (true) {
		htcmdbuf[0] = 0x02;		// set write to channel
		htcmdbuf[1] = 0x4D;		// set digital outputs B
		htcmdbuf[2] = state;	// pass the state
		I2CBytes(PROTO_PORT,htcmdbuf,htcount,htrspbuf);
	}
}
// showstate

/*!
	\brief Starts debugging tasks
	
	Starts all the debugging tasks. Is started in the main
	task if DEBUG is defined.
*/
task debug (void)
{
	start ledflash;
	start robotstatus;
	start showstate;
}