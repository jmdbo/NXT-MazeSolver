/*! \file libNXC.h
	\brief NXC library and wrappers for libNBC.h
	
	Provides library functions written in NXC and
	wrapper macros for the routines / macros
	provided by libNBC.h
	
	\author thomas.zink
	\version 20101123
	
	Changelog:
		- 20110517 thomas.zink
			- work on comments
		- 20101123 thomas.zink
			- added NXC versions of lib functions
			- moved to doxygen style comments
		- 20100114 thomas.zink
			- rotations2degrees wrapper
			- removed c verification function, redundant
		- 20100112 thomas.zink
			-initial version
*/
#ifndef LIBNXC__H
#define LIBNXC__H 1

#define USENBC 1      //!< use the libnbc library instead of libnxc

#if USENBC == 0
/*!
	\brief Builds an array of output ports.
	
	Macro to dynamically build an array of output ports.
	When using variables to pass output ports to functions,
	the NXC/NBC output module requires passing an array
	instead of a byte. Passing the constant definitions through
	a variable does not work. The array must be manually built.
	This macro builds an array according to the output port
	constant.
	
	\param		arr			The array which will be build
	\param		outs		The output constant
*/
#define ArrayBuildPort(arr, outs) { \
	if (outs <= OUT_C) { ArrayBuild(arr,outs); } \
	else if (outs == OUT_AB) { ArrayBuild(arr,OUT_A,OUT_B); } \
	else if (outs == OUT_AC) { ArrayBuild(arr,OUT_A,OUT_C); } \
	else if (outs == OUT_BC) { ArrayBuild(arr,OUT_B,OUT_C); } \
	else if (outs == OUT_ABC) { ArrayBuild(arr,OUT_A,OUT_B,OUT_C); } \
}

/*!
	\brief Rotate motor(s) such that wheel turn a number of mm.
	
	Rotate motor(s) a number of degrees, such that the
	attached wheels drive a distance of the specified
	milli meters.
	
	\param	outputs		Desired output ports. Can be a constant or a variable.
										Either way an output array will automatically be built.
	\param 	pwr				Output power, 0 to 100. Can be negative to reverse direction.
	\param	mm				Number of milli meters to turn the wheel(s).
	\param	circ			Circumference of the wheel(s) in mm.
	\return Number of degrees the wheel(s) have been turned.
*/
unsigned long RotateMotorMm (byte outputs, char pwr, int mm, unsigned int circ)
{
	byte ports[];
	ArrayBuildPort(ports, outputs);
	unsigned long degrees = ((mm*360) / circ);
	RotateMotor(ports, pwr, degrees);
	return(degrees);
}
// RotateMotorMm

/*!
	\brief Rotate two parallel wheels such that they turn on a base circle a number of degrees.
	
	Rotates two parallel wheels a number of rotations such that they turn on
	the base circle a number of degrees. This can be used to turn a two wheel
	robot in either direction a number of degrees. The function does
	not check parameters for correctness. You must check yourself to
	pass a valid output specifier.
	
	\param	outputs		Desired output ports. Can be a constant or a variable.
										Either way an output array will automatically be built
	\param	pwr				Output power, 0 to 100. Can be negative to reverse direction.
	\param	degrees		Number of degrees to turn. Can be negative to reverse direction
	\param	diam			Diameter of the wheels
	\param	ccdist		Center-to-center distance between the wheels
	\return Number of rotations performed by the wheels
*/
unsigned long RotateBaseDegrees (byte outputs, char pwr, int degrees, unsigned int diam, unsigned int ccdist) 
{
	// don't care for that
	byte ports[];
	ArrayBuildPort(ports, outputs);
	byte port0 = ports[0];
	byte port1 = ports[1];
	int turnpct = (degrees >= 0) ? 100 : -100;
	unsigned long rotations = (abs(degrees) * ccdist) / diam;
	// following same as (not working due to counter reset issues):
	//RotateMotorEx(ports, pwr, rotations, turnpct, TRUE, TRUE);
	// while the Motor Rotation Counts != 0 {
	while((MotorRotationCount(port0) != 0) || (MotorRotationCount(port1) != 0)) {
		ResetRotationCount(port0);
		ResetRotationCount(port1);
	};
	OnFwdSync(ports,pwr,turnpct);
	while ((abs(MotorRotationCount(port0)) <= rotations) || (abs(MotorRotationCount(port1)) <= rotations));
	Off(ports);
	return (rotations);
}

#endif // USENBC == 0
#if USENBC == 1

// WRAPPER MACROS FOR NBC ROUTINES / MACROS
// these must precede the asm include headers.
#define Degrees2Rotations(_deg) 			\
asm { 										\
	__Degrees2Rotations(_deg, __RETVAL__) 	\
}

#define Rotations2Degrees(_rot) 			\
asm { 										\
	__Rotations2Degrees(_rot, __RETVAL__) 	\
}

#define Mm2Degrees(_mm,_circ) 			\
asm { 									\
	__Mm2Degrees(_mm,_circ,__RETVAL__)	\
}

#define ArrayBuildPort(_arr, _ports) \
asm { \
	__ArrBuildPort(_arr, _ports) \
}

#define RotateMotorMm(_ports, _pwr, _mm, _circ)				\
asm { 														\
	__RotateMotorMm(_ports, _pwr, _mm, _circ, __RETVAL__)	\
}

#define RotateBaseDegrees(_ports,_pwr,_degrees,_diam,_ccdist) 				\
asm {																		\
	__RotateBaseDegrees(_ports,_pwr,_degrees,_diam,_ccdist,__RETVAL__)	\
}


asm {
	#include "libNBC.h"
}

#endif // USENBC == 1

#endif // LIBNXC_H