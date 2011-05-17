/*! \file libNBC.h
	\brief NBC library functions
	
	Provides library functions written in NBC.
	This file adds useful macro functions to the standard NBC API,
	like rotating the robot a certain amount of degrees
	and moving a certain amount of cm forward.
	
	There are wrappers available in libNXC.h that wrap
	these functions and make them available in NXC.

	\author thomas.zink
	\version 20101123
*/
#ifndef LIBNBC__H
#define LIBNBC__H 1

/*
	\name Degrees2Rotations
	Takes a number of degrees and calculates the number of rotations
	needed to turn the motor(s). An equivalent NXC function looks like
	this:
	
	int Degrees2Rotations (int degrees) { return degrees / 360; }
	
	\param		_deg		Number of degrees
	\param		_R			Return variable
	\return		Number of rotations
*/

// DATA
dseg segment
	__deg2rot_mutex       mutex   //!< mutex to ensure mutual exclusion
	__deg2rot_rotations   slong   //!< rotations
	__deg2rot_degrees     slong   //!< degrees
dseg ends

// SUBROUTINE
subroutine __deg2rot_sub
	set    __deg2rot_rotations,   360                                       // rotations = 360
	div    __deg2rot_rotations,   __deg2rot_degrees,    __deg2rot_rotations // rotations = degrees / rotations
	return
ends

// MACRO
#define __Degrees2Rotations(_deg,_R) \
	acquire  __deg2rot_mutex \
	mov      __deg2rot_degrees, _deg \
	call     __deg2rot_sub \
	mov      _R,                __deg2rot_rotations \
	release  __deg2rot_mutex

// WRAPPER
#ifndef Degrees2Rotations
#define Degrees2Rotations(_deg,_R) \
	__Degrees2Rotations(_deg,_R)
#endif

/*
	\name Rotations2Degrees
	Calculates the number of degrees a motor turned depending on the number of
	rotations. An equivalent NXC function would look like this:
	
	int Rotations2Degrees (int rotations) { return rotations * 360; }
	
	\param 	_rot 	Number of rotations
	\param	_R		Desired return variable
	\return Number of degrees the motor turned
*/

// DATA
// we don't need a data segment. We will use the same data as Degrees2Rotations

// SUBROUTINE
subroutine __rot2deg_sub
	set    __deg2rot_degrees,   360                                     // degrees = 360
	mul    __deg2rot_degrees,    __deg2rot_degrees,	__deg2rot_rotations // degrees = degrees * rotations
	return
ends

// MACRO
#define __Rotations2Degrees(_rot,_R) \
	acquire  __deg2rot_mutex \
	mov      __deg2rot_rotations,  _rot \
	call     __rot2deg_sub \
	mov      _R,                   __deg2rot_degrees \
	release  __deg2rot_mutex

// WRAPPER
#ifndef Rotations2Degrees
#define Rotations2Degrees(_rot,_R) \
	__Rotations2Degrees(_rot,_R)
#endif

/*!
	\name ArrayBuildPort
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
#define __ArrBuildPort(_arr, _ports) \
	compif		EQ,		isconst(_ports), 	FALSE		\
	arrbuild	_arr,	_ports										\
	compelse																	\
	compchk		GT,		_ports,				0x02				\
	compchk		LT,		_ports,				0x07				\
	__ArrBuildPort_##_ports(_arr)						\
	compend																		\

#define __ArrBuildPort_3(_arr)						\
	arrbuild	_arr,		OUT_A, OUT_B

#define __ArrBuildPort_4(_arr)						\
	arrbuild	_arr,		OUT_A, OUT_C

#define __ArrBuildPort_5(_arr)						\
	arrbuild	_arr,		OUT_B, OUT_C

#define __ArrBuildPort_6(_arr)						\
	arrbuild	_arr,		OUT_A, OUT_B, OUT_C

#ifndef ArrayBuildPort
	#define ArrayBuildPort(_arr, _ports) \
	__ArrBuildPort(_arr, _ports)
#endif

/*!
	\brief Rotate motor(s) such that wheel(s) turn a number of mm.
	
	Rotate motor(s) a number of degrees, such that the
	attached wheels drive a distance of the specified
	milli meters. An equivalent NXC function has the following
	declaration:

	unsigned long RotateMotorMm (byte outputs, char pwr, int mm, unsigned int circ);
	
	\param	_ports		Desired output ports. Can be a constant or a variable.
	\param 	_pwr			Output power, 0 to 100. Can be negative to reverse direction.
	\param	_mm				Number of milli meters to turn the wheel(s).
	\param	_circ			Circumference of the wheel(s) in mm.
	\param 	_R				Desired return variable
	\return Number of degrees the wheel(s) have been turned.
*/
// DATA
dseg segment
	__rotateMMm_mutex		mutex		//!< mutex
	__rotateMMm_result	ulong		//!< return value degrees (_R)
	__rotateMMm_mm			sword		//!< parameter distance in mm (_mm)
	__rotateMMm_circ		uword		//!< circumference in mm (_circ)
	/* we don't need a variable for _ports and _pwr since we will pass them
		directly to RotateMotor()	*/
dseg ends

// SUBROUTINE
subroutine __rotateMMm_sub
	// compute degrees = ((mm*360) / circ);
	set 	__rotateMMm_result, 	360																				// result = 360
	mul		__rotateMMm_result, 	__rotateMMm_result, 	__rotateMMm_mm			// result = result * mm
	div 	__rotateMMm_result, 	__rotateMMm_result, 	__rotateMMm_circ		// result = result / circumference
	return
ends

// MACRO
#define __RotateMotorMm(_ports, _pwr, _mm, _circ, _R)							\
	acquire			__rotateMMm_mutex 																	\
	mov					__rotateMMm_mm, 		_mm															\
	mov					__rotateMMm_circ, 	_circ														\
	call __rotateMMm_sub \
	RotateMotor(_ports, _pwr, __rotateMMm_result); \
	mov					_R,									__rotateMMm_result							\
	release			__rotateMMm_mutex
	
// WRAPPER
#ifndef RotateMotorMm
#define RotateMotorMm(_ports, _pwr, _mm, _circ, _R) \
	__RotateMotorMm(_ports, _pwr, _mm, _circ, _R)
#endif

/*!
	\brief Rotate two parallel wheels such that they turn on a base circle a number of degrees.
	
	Rotates two parallel wheels a number of rotations such that they turn on
	the base circle a number of degrees. This can be used to turn a two wheel
	robot in either direction a number of degrees. The function does
	not check parameters for correctness. You must check yourself to
	pass a valid output specifier. A corresonding NXC function has the
	following declaration:
	
	unsigned long RotateBaseDegrees (byte outputs, char pwr, int degrees, unsigned int diam, unsigned int ccdist);
	
	\param	_ports			Desired output ports. Can be a constant or a variable.
											Either way an output array will automatically be built
	\param	_pwr				Output power, 0 to 100. Can be negative to reverse direction.
	\param	_degrees		Number of degrees to turn. Can be negative to reverse direction
	\param	_diam				Diameter of the wheels
	\param	_ccdist			Center-to-center distance between the wheels
	\return Number of rotations performed by the wheels
*/
// DATA
dseg segment
	__rotbase_mutex			mutex
	// output ports
	__rotbase_ports			ubyte[]
	__rotbase_port0			ubyte
	__rotbase_port1			ubyte
	// parameters
	__rotbase_pwr				ubyte
	__rotbase_degrees		sword
	__rotbase_diam			uword
	__rotbase_ccdist		uword
	__rotbase_result		ulong
dseg ends

// SUBROUTINE
subroutine __rotbase_sub
	// local data segment
	dseg segment
		degrees		uword
		turnpct		sbyte
		rot0			sword
		rot1			sword
	dseg ends
	// turnpct = sign(__rotbase_degrees) * 100
	sign	turnpct,	__rotbase_degrees																	// turnpct = sign(__rotbase_degrees)
	mul		turnpct,	turnpct,	100																			// turnpct = turnpct * 100
	// degrees = abs(__rotbase_degrees)
	// result = (degrees * __rotbase_ccdist) / __rotbase_diam
	abs		degrees,	__rotbase_degrees																	// degrees = abs(__rotbase_degrees)
	mul		__rotbase_result,		degrees,	__rotbase_ccdist							// result = degrees * __rotbase_ccdist
	div		__rotbase_result,		__rotbase_result,		__rotbase_diam			// result = result / __rotbase_diam
	// we could do instead
	// RotateMotorEx(__rotbase_ports, __rotbase_pwr, __rotbase_result, turnpct, TRUE, TRUE);
	// wait_loop {
	wait_loop: // {
		// reset rotation counters for __rotbase_port{0,1}
		setout 	__rotbase_port0,	UpdateFlags,			RESET_ROTATION_COUNT	// ResetRotationCount(__rotbase_port0)
		setout 	__rotbase_port1,	UpdateFlags,			RESET_ROTATION_COUNT	// ResetRotationCount(__rotbase_port1)
		// get the motor rotation counters for __rotbase_port{0,1} and store them in rot{0,1}
		getout 	rot0, 						__rotbase_port0,	RotationCount					// rot0 = MotorRotationCount(__rotbase_port0)
		getout 	rot1, 						__rotbase_port1,	RotationCount					// rot1 = MotorRotationCount(__rotbase_port1)
		// if rot0 != 0 || rot1 != 0 goto wait_loop
		brtst		NEQ,							wait_loop,				rot0									// if rot0 != 0 goto wait_loop
		brtst		NEQ,							wait_loop,				rot1									// if rot1 != 0 goto wait_loop
	//}
	// start motors synchronized
	OnFwdSync(__rotbase_ports, __rotbase_pwr, turnpct);
	// setout __rotbase_ports, Power, __rotbase_pwr, OutputMode, OUT_MODE_MOTORON+OUT_MODE_REGULATED+OUT_MODE_BRAKE, RegMode, OUT_REGMODE_SYNC, TurnRatio, __rotbase_turnpct, RunState, OUT_RUNSTATE_RUNNING, TachoLimit, 0, UpdateFlags, UF_UPDATE_TACHO_LIMIT+UF_UPDATE_MODE+UF_UPDATE_SPEED+RESET_ALL
	// while_loop {
	while_loop: // {
		// get the abs of motor rotation counters ports{0,1} and stor in rot{0,1}
		getout 	rot0, 					__rotbase_port0,	RotationCount					// rot0 = abs(MotorRotationCount(__rotbase_port0))
		abs			rot0,						rot0
		getout 	rot1, 					__rotbase_port1,	RotationCount					// rot1 = abs(MotorRotationCount(__rotbase_port1))
		abs			rot1,						rot1
		// if rot0 < result || rot 1 < result goto while_loop
		brcmp		<, while_loop,	rot0,		__rotbase_result			// if rot0 < result || rot 1 < result goto while_loop
		brcmp		<, while_loop,	rot1,		__rotbase_result
	//}
	// shut off the motors
	Off(__rotbase_ports)
	// return
	return
ends

// MACRO
#define __RotateBaseDegrees(_ports,_pwr,_degrees,_diam,_ccdist,_R) 	\
	acquire	__rotbase_mutex														\
	mov			__rotbase_pwr,			_pwr									\
	mov			__rotbase_degrees,	_degrees							\
	mov			__rotbase_diam,			_diam									\
	mov			__rotbase_ccdist,		_ccdist								\
	__ArrBuildPort(__rotbase_ports, _ports);					\
	index		__rotbase_port0,		__rotbase_ports,		0	\
	index		__rotbase_port1,		__rotbase_ports,		1	\
	call		__rotbase_sub															\
	mov			_R,									__rotbase_result			\
	release	__rotbase_mutex

// WRAPPER
#ifndef RotateBaseDegrees
#define RotateBaseDegrees(_ports,_pwr,_degrees,_diam,_ccdist,_R) \
	__RotateBaseDegrees(_ports,_pwr,_degrees,_diam,_ccdist,_R)
#endif

#endif // LIBNBC__H
