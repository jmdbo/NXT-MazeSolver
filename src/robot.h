/*! \file robot.h
	\brief Robot definitions and functions
	
	Robot definitions like input/output ports, speed, metrics and so on.
	Provides the function init which initializes all sensors.
	
	The base model of the robot is the standard NXT Education Set
	model with slight modifications to the sensor array, which is built
	to house the HT color sensor parallel to surface. If the Lego color
	sensor should be used, this file needs to be adjusted.
	
	\author thomas.zink
	\version 20101123
	
	Changelog:
		- 20101123 thomas.zink
			- moved to doxygen comments
			- some port redefinitions
			- some global variable changes
		- 20100113 thomas.zink
			- initial version
*/
#ifndef ROBOT_H
#define ROBOT_H 1

// SENSORS
#define COLOR_PORT    IN_1                           //!< HT color sensor port
#define COLOR_VALUE   SensorHTColorNum( COLOR_PORT ) //!< current HT color sensor value
#define PROTO_PORT    IN_2                           //!< HT protoype port
#define LIGHT_PORT    IN_3                           //!< light sensor port
#define LIGHT_VALUE   SensorValue(LIGHT_PORT)        //!< current light sensor value
#define TOUCH_PORT    IN_4                           //!< touch sensor port
#define TOUCH_VALUE   SensorValue(TOUCH_PORT)        //!< current touch sensor value


// ACTUATORS
#define MOTOR_LEFT    OUT_A    //!< left motor port
#define MOTOR_RIGHT   OUT_C    //!< right motor port
#define MOTOR_BOTH    OUT_AC   //!< both motor ports
#define LED           OUT_B    //!< status LED output port

// SPEEDS
// some speed definitions
#define SPEED_SLOW     30     //!< slow speed 
#define SPEED_MEDIUM   50     //!< medium speed
#define SPEED_HIGH     70     //!< high speed
#define SPEED_MAX      100    //!< maximum speed

// METRICS
// metric information about the Robot
#define DIAM    56     //!< the wheel diameter
#define CIRC    176    //!< circumference of a wheel
#define CDIST   115    //!< center-to-center wheel distance
#define SDIST   60     //!< distance of the axis to surface sensor

// GLOBALS
byte htcmdbuf[];       //!< command buffer for HT proto board
byte htrspbuf[];       //!< response buffer for HT proto board
int htcount = 0;       //!< counter for HT proto board

/*!
	\brief Initializes Sensors

	Initializes the sensors and the HT prototype board.
*/
void init (void)
{
	SetSensorLowspeed(COLOR_PORT);
	SetSensorType(LIGHT_PORT, SENSOR_TYPE_LIGHT_ACTIVE);
	SetSensorMode(LIGHT_PORT, SENSOR_MODE_PERCENT);
	ResetSensor(LIGHT_PORT);
	SetSensorTouch(TOUCH_PORT);
	SetSensorLowspeed(PROTO_PORT);
	Wait(100);
	ArrayInit(htcmdbuf,0,3);
	htcmdbuf[0] = 0x02;		// set write
	htcmdbuf[1] = 0x4E;		// B controls
	htcmdbuf[2] = 0x3F;		// write 00111111
	I2CBytes(PROTO_PORT,htcmdbuf,htcount,htrspbuf);
}
// init

#endif // ROBOT_H
