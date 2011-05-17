/*! \file world.h
	\brief Definitions and tasks about the world

	World definitions. World functions. World tasks.
	Everything related and dependant on the world
	the robot lives in should be defined here.
	
	The mazes have the following properties:
	- rectangular grid of lines and junctions on 1m^2
	- colors of lines, background, junction, exit differ
		for different maze types
	
	The Robot performs the following task in the world:
		- observe (observe the surface)
	
	Note that depending on MAZE_TYPE definitions of constants and
	tasks differ.
	
	\author thomas.zink
	\version 20101123
	
	Changelog:
		- 20110517 thomas.zink
			- work on comments
		- 20101123 thomas.zink
				- redesign, more comments
				- moved to doxygen style commentary
		- 20100114 thomas.zink
				- initial version
*/
#ifndef WORLD_H
#define WORLD_H 1

// MAZE TYPES
// which type of maze is used
// cannot use doxygen comments here, will result in
// invalid preprocessor statement
#define MAZE_WHITE    0x01             //!< old-style maze with white background
#define MAZE_GRAY     0x02             //!< new-style maze with gray background
#define MAZE_COLOR    0x03             //!< brand-new color mazes
#define MAZE_TYPE     MAZE_COLOR       //!< maze type used

// SURFACE
// what surface the robot observes
#define SURFACE_LINE    0x01          //!< above a line
#define SURFACE_JUNC    0x02          //!< hit a junction
#define SURFACE_EXIT    0x03          //!< above the exit (hoooray)
#define SURFACE_NDEF    0x04          //!< everything else is undefined
byte surface;                         //!< set by observe to one of the surface definitions

#if MAZE_TYPE == MAZE_WHITE
/*
	MAZE_WHITE
	background: white
	lines: grey
	junctions: black
	exit: black
*/
// LIGHT
#define LIGHT_DELTA    5     //!< bias to adjust light values
#define LIGHT_JUNC     33    //!< reflected light on junction
#define LIGHT_LINE     50    //!< reflected light on line

int tjunc = LIGHT_JUNC + LIGHT_DELTA;    //!< light threshold for junctions
int tline = LIGHT_LINE - LIGHT_DELTA;    //!< light threshold for lines
int tndef = LIGHT_LINE + LIGHT_DELTA;    //!< light threshold for undefined

// METRICS
#define LEN_JUNC    35       //!< length of a junction
#define WID_JUNC    35       //!< width of a junction
#define LEN_LINE    141      //!< length of a line
#define WID_LINE    17       //!< width of a line

/*!
	\brief Observe the surface
	
	Checks the surface and sets the global variable `surface`
	to the value corresponding to the tone the sensor
	sees.
	\see surface
*/
task observe (void)
{
	int light;
	while (true) {
		light = LIGHT_VALUE;
		if (light < tjunc) surface = SURFACE_JUNC;
		else if ((light > tline) && (light < tndef)) surface = SURFACE_LINE;
		else surface = SURFACE_NDEF;
	}
}
// observe

#endif	// MAZE_WHITE
/**************************************************************************/
#if MAZE_TYPE == MAZE_GRAY
/*
	MAZE_GRAY
	background: gray
	lines: white
	junctions: black
	exit: black
*/
// LIGHT
#define LIGHT_DELTA    5     //!< bias to adjust light values
#define LIGHT_JUNC     40    //!< reflected light on junction
#define LIGHT_LINE     67    //!< reflected light on line

int tjunc = LIGHT_JUNC + LIGHT_DELTA; //!< light threshold for junctions
int tline = LIGHT_LINE - LIGHT_DELTA; //!< light threshold for lines

// METRICS
#define LEN_JUNC    27        //!< length of a junction
#define WID_JUNC    27        //!< width of a junction
#define LEN_LINE    113       //!< length of a line
#define WID_LINE    21        //!< width of a line

/*!
	\brief Observe the surface
	
	Checks the surface and sets the global variable `surface`
	to the value corresponding to the tone the sensor
	sees.
	\see surface
*/
task observe (void)
{
	int light;
	while (true) {
		light = LIGHT_VALUE;
		if (light < tjunc) surface = SURFACE_JUNC;
		else if (light > tline) surface = SURFACE_LINE;
		else surface = SURFACE_NDEF;
	}
}
// observe

#endif 	// MAZE_GRAY
/**************************************************************************/
#if MAZE_TYPE == MAZE_COLOR
/*
	MAZE_COLOR
	background: white
	lines: black
	junctions: red
	exit: blue
*/
// METRICS
#define LEN_JUNC    30        //!< length of a junction
#define WID_JUNC    30        //!< width of a junction
#define LEN_LINE    110       //!< length of a line
#define WID_LINE    20        //!< width of a line

/*!
	\brief Observe the surface
	
	Checks the surface and sets the global variable `surface`
	to the value corresponding to the tone the sensor
	sees.
	\see surface
*/
task observe (void)
{
	int color;
	while (true) {
		color = COLOR_VALUE;
		switch (color) {
			case 0:
				surface = SURFACE_LINE;
				break;
			case 2:
			case 3:
				surface = SURFACE_EXIT;
				break;
			case 7:
			case 8:
			case 9:
			case 10:
				surface = SURFACE_JUNC;
				break;
			default:
				surface = SURFACE_NDEF;
				break;
		}
	}
}
// observe
#endif	// MAZE_COLOR

#endif // WORLD_H
