\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Meme bottom-level definitions.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

\
\ The code in this file is loaded both for the standard library and for
\ the main dictionary.  The code in meme.m is loaded for the main
\ dictionary only.
\

1024 constant maxobjects	\ Change MAXOBJECTS in meme.h when you change this

\ Object type and subtype definitions
0	constant	cameraObject
1	constant	lightObject
2	constant	visibleObject
( 3	constant	audibleObject )
4	constant	imaginaryObject
5	constant	compoundObject
6	constant	spriteObject

\ Light subtypes
0	constant	directionalLight
1	constant	conicalLight
2	constant	pointLight
3	constant	parallelPointLight
\ 4	constant	ambientLight

\ Visible object subtypes
5	constant	plainVisible
6	constant	containerVisible

\ Camera subtypes
1 8 <<				constant	monoCamera
2 8 <<				constant	stereoCamera
stereoCamera 1 +	constant	stereoInterlacedLRCamera
stereoCamera 2 +	constant	stereoInterlacedRLCamera
stereoCamera 3 +	constant	stereoAdjacentLRCamera
stereoCamera 4 +	constant	stereoAdjacentRLCamera
3 8 <<				constant	textureCamera

\ Texture map wrap types
0 	constant	noWrap
1	constant	sphericalWrap
2	constant	cylindricalWrap
3	constant	cubicalWrap
4	constant	flatWrap

\ Shading types
0	constant	flatShaded
1	constant	smoothShaded
\ 2	constant	phongShaded

\ Geometry types
0 8 <<	constant	pointGeometry
1 8 <<	constant	wireGeometry
2 8 <<	constant	solidGeometry

\ Link types
0	constant	lockedLink
1	constant	swivelLink

\ Return a pointer to the world object (first object in the array)
: world 0 object ;

\ This is the struct corresponding to a virtual object
BeginStruct
	Int		ptr				\ Ptr to native struct for camera, light, shape, etc.
	Int		frame			\ Ptr to local xform matrix (Rendermorphics RL only)
	Float	x				\ X,
	Float	y				\ Y,
	Float	z				\ Z position components
	Float	yaw				\ Yaw angle--rotation around the Y (vertical) axis
	Float	pitch			\ Pitch angle--rotation around the X axis
	Float	roll			\ Roll angle--rotation around the Z axis
	Float	xscale			\ X scale factor
	Float	yscale			\ Y scale factor
	Float	zscale			\ Z scale factor
	Int		child			\ Ptr to virtual object struct for my child
	Int		sibling			\ Ptr to virtual object struct for my last sibling
	Int		backptr
	Int		parent
	Int		objecttype
	Int		objectsubtype
	Int		linktype
	Int		used
Struct VirtualObjects

\ Some utility functions
: f? f@ f. ;
: f+! dup f@ f+ f! ;

\ : ops -1 over opsChanged ! ;
: ops ; immediate	\ dummy definition now
\ Memes for accessing virtual object fields
: x@		x f@ ;
: x!		ops x f! ;
: x+!		ops x f+! ;
: y@		y f@ ;
: y!		ops y f! ;
: y+!		ops y f+! ;
: z@		z f@ ;
: z!		ops z f! ; 
: z+!		ops z f+! ;
: yaw@		yaw f@ ;
: yaw!		ops yaw f! ;
: yaw+!		ops yaw f+! ;
: pitch@	pitch f@ ;
: pitch!	ops pitch f! ;
: pitch+!	ops pitch f+! ;
: roll@		roll f@ ;
: roll!		ops roll f! ;
: roll+!	ops roll f+! ;
: xscale@	xscale f@ ;
: xscale!	ops xscale f! ;
: xscale+!	ops xscale f+! ;
: yscale@	yscale f@ ;
: yscale!	ops yscale f! ;
: yscale+!	ops yscale f+! ;
: zscale@	zscale f@ ;
: zscale!	ops zscale f! ;
: zscale+!	ops zscale f+! ;

: xyz@		\ objaddr -- | [float] -- x y z
	dup x@ dup y@ z@
;

: xyz!		\ objaddr -- | [float] x y z --
	dup z! dup y! x!
;

: scale@	\ objaddr -- | [float] -- xscale yscale zscale
	dup xscale@ dup yscale@ zscale@
;

: scale!	\ objaddr -- | [float] xscale yscale zscale --
	dup zscale! dup yscale! xscale!
;

: ypr@		\ objaddr -- | [float] -- yaw pitch roll
	dup yaw@ dup pitch@ roll@
;

: ypr!		\ objaddr -- | [float] yaw pitch roll --
	dup roll! dup pitch! yaw!
;

: transform@	\ objaddr -- | [float] -- x y z xscale yscale zscale y p r
	dup xyz@ dup scale@ ypr@
;

: transform!	\ objaddr -- | [float] x y z xscale yscale zscale y p r --
	dup ypr! dup scale! xyz!
;

: identity!		\ objaddr --
	0e  fdup fdup fdup  dup ypr!  fdup fdup  dup xyz!
	1e fdup fdup scale!
;

\ Colors
: grayColor 0.5e 0.5e 0.5e ;
: blueColor 0.0e 0.0e 1.0e ;
: greenColor 0.0e 1.0e 0.0e ;
: redColor 1.0e 0.0e 0.0e ;
: brownColor 0.3e 0.3e 0.0e ;
: yellowColor 1.0e 1.0e 0.0e ;

\ Mouse button constants
0 constant noButton
1 constant leftButton
2 constant rightButton
4 constant middleButton

\ Joystick button constants
1 constant triggerButton
2 constant thumbButton

\ Mouse stuff
create MouseVector ] pause pause pause pause pause pause pause pause [
: MouseVector! ( cfa bitmask -- )
	/n* MouseVector + !
;
: OnMouseButtons	( bitmask -- )	\ next word in input stream is run
	'				( bitmask cfa )
	swap MouseVector!		\ write the cfa in the table
;
: DoMouse		( -- )		\ Run this word in a background loop
	MouseB /n* MouseVector + @ execute 
;
: NormalizedMouseX
	MouseX     WindowWidth 2/
	dup >r
	- float		\ a # in interval [ -(WindowWidth/2), WindowWidth/2-1 ]
	r> 1- float	\ get WindowWidth/2-1
	f/ 		\ divide
	-1e fmax	\ clamp to -1 minimum
;
: NormalizedMouseY	( [float] -- MouseY )
	MouseY     WindowHeight 2/
	dup >r
	swap - float	\ # in interval [ -(WindowHeight/2), WindowHeight/2-1 ]
	r> 1- float	\ get WindowHeight/2-1
	f/ 		\ divide
	-1e fmax	\ clamp to -1 minimum
;

\ Joystick stuff
create JoystickVector ] pause pause pause pause [
: JoystickVector!	( cfa bitmask -- )
	/n* JoystickVector + !
;
: OnJoystickButtons	( bitmask -- )	\ next word in input stream is run
	'				( bitmask cfa )
	swap JoystickVector! 		\ write the cfa in the table
;
: DoJoystick		( -- )		\ Run this word in a background loop
	JoystickB /n* JoystickVector + @ execute 
;

\ Re-scales an object by a scale factor
: rescale	( float objaddr -- )
	fdup dup xscale@ f* dup xscale!
	fdup dup yscale@ f* dup yscale!
	dup zscale@ f* zscale! ;

\ Sets the scale of an object
\ ( objaddr -- )
\ ( [float] scale -- )
: scale
	dup fdup xscale! dup fdup yscale! zscale!
;


BeginStruct
	Float	sAngle
	Float	sDeltaAngle
	Float	sAmplitude
	Float	sBias
Struct sinerData

: siner	( [float] period amplitude bias phase -- )
    create
	here >r
	[SizeOf] sinerData allot
	r@ sAngle f!
	r@ sBias f!
	r@ sAmplitude f!
	twoPi fswap f/ r> sDeltaAngle f!
    does>
	dup sAngle f@ fsin
	dup sAmplitude f@ f*
	dup sBias f@ f+
	dup sDeltaAngle f@ dup sAngle f@ f+ twoPi fmod
	sAngle f!
;

\ ***************************************************************************
: frand	( [float] -- f ) \ Return floating point random # 0 <= x < 1
	Rand th 7fffffff and float 2^31 f/
;
\ ***************************************************************************
\ For executing a meme in the main dictionary from a module.  If the flag
\ is true, addr is a cfa that is passed to "execute".  If the flag is
\ false, the outer interpreter is reentered via abort".
\ Usage: p" memename" find ?execute
: ?execute ( addr flag -- )
	0= abort" Couldn't execute looked-up meme "
	execute
;
\ This meme pushes the main task's IP on the return stack, and sets
\ a new IP.  It causes the main task to interrupt what it's doing and
\ run the colon definition pointed at by newip.  The main task will resume
\ where it left off when the ; of the colon definition is reached.
\ This meme is used by a background task in a module that wants to chain
\ to another module.  The code that chains modules must not have its stacks
\ in the exiting module, so the main task is preempted to do the chaining.
\ : preempt ( newip -- )
\     up@ maintask <>
\ 	if	\ If maintask isn't executing this
\ 
\ 		\ Put maintask to sleep (so we can debug through it)
\ 		maintask sleep
\ 
\ 		\ Save the main task's IP on its return stack
\ 		/n negate maintask saved-rp local +!
\ 		maintask saved-ip local @   maintask saved-rp local @   !
\ 
\ 		\ Set the main task's IP to newip
\ 		maintask saved-ip local !
\ 	
\ 		\ Wake maintask
\ 		maintask wake
\ 
\ 		\ Immediately pass control to the main task
\ 		>maintask
\ 
\ 	then
\ ;
\ ***************************************************************************
\ This is the structure returned by GetTimeOfDay
BeginStruct
	Int	todSeconds
	Int	todMinutes
	Int	todHours
	Int	todDayOfMonth
	Int	todMonth
	Int	todYear
	Int	todDayOfWeek
	Int	todDayOfYear
	Int	todDaySaveTime
Struct TimeOfDays
\ ***************************************************************************
\ These are time routines.

\ Return the number of seconds since January 1, 1970 as a floating-point
\ number.  For DOS systems this has 1/100 of a second precision.
: fGetTime ( [float] -- time )
	GetTime			\ ( -- seconds milliseconds )
	float 1000e f/ 	\ ( -- seconds ) ( [float] -- partial.seconds )
	float f+		\ ( -- ) ( [float] -- seconds )
;

\ Wait n milliseconds.  Yield to other tasks while waiting.
: ms	\ milliseconds --
	float 1000e f/ fGetTime f+		\ Compute the time in the future at which we
									\ exit the loop
	begin
		fdup fGetTime f>
	while							\ While the computed time 
									\ comes after the current time

		pause						\ Switch context
	repeat							\ Loop to 'begin'
	fdrop
;

\ The first (newer) TimeScaler, below, has a low-lag filter
\ implemented that smooths its operation.

\ This is a defining meme.  A defined meme takes a floating-point argument
\ which is multiplied by the number of seconds since the last time the
\ defined meme was called.
: TimeScaler ( -- )
	create
	fGetTime f,				\ Current time.
	0.1e f,					\ Running avg.  Start it at about the time it takes
							\	between frames.
							\ does> portion: ( [float] n -- n*seconds )
	does>					( -- addr ) \ n is on float stk
	{ | n ; time lastime deltatime avg ratio weight }
	fGetTime to time
	dup f@ to lastime
	dup /f + f@ to avg
	time lastime f- to deltatime
	deltatime f0<>
	if
		time dup f!
		deltatime avg f- fabs avg f/ to ratio
		ratio 0.2e f> if 
			0.9e to weight
		else
			0.4e to weight
		then
		deltatime weight f* 			\ Multiply the delta by the weight
		avg 1e weight f- f*				\ Multiply the avg delta by 1-weight
		f+ 								\ Add 'em
		fdup /f + f!		
	else
		drop avg
	then
	n f*						\ ( [float] -- scaled.n )
;

\ This is a defining meme.  A defined meme takes a floating-point argument
\ which is multiplied by the number of seconds since the last time the
\ defined meme was called.
\ : TimeScaler ( -- )
\ 	create
\ 	fGetTime f,				\ Current time.
\ 	0.1e f,					\ Running avg.  Start it at about the time it takes
\ 							\	between frames.
\ 							\ does> portion: ( [float] n -- n*seconds )
\ 	does>					\ ( -- addr )
\ 	fGetTime fdup			\ ( -- addr ) ( [float] -- n time time )
\ 	dup f@					\ ( -- addr ) ( [float] -- n time time lastime )
\ 	f-						\ ( -- addr ) ( [float] -- n time deltatime )
\ 	fdup f0<>				\ ( -- addr flag ) ( [float] -- n time deltatime )
\ 	if						\ If delta time not zero
\ 		dup fswap f!		\ ( -- addr ) ( [float] -- n deltatime )
\ 		dup /f + f@ 		\ ( -- addr ) ( [float] -- n delta avg )
\ 		f+ 2e f/			\ ( -- addr ) ( [float] -- n newavg )
\ 		fdup /f + f!		\ ( -- ) ( [float] -- n newavg )
\ 	else					\ If delta time is zero
\ 		fdrop fdrop 		\ ( -- addr ) ( [float] -- n )
\ 		/f + f@				\ ( -- ) ( [float] -- n avgdelta )
\ 	then
\ 	f*						\ ( [float] -- scaled.n )
\ ;

\ Initialize a TimeScaler.  The floating-point value is used to initialize
\ the running average.  The time field is initialized to the current time.
: InitTimeScaler ( cfa -- ) ( [float] n -- )
	>body					\ ( -- pfa ) ( [float] -- n )
	fGetTime dup f!			\ ( -- pfa ) ( [float] -- n )
	/f + f!					\ ( -- ) ( [float] -- )
;


BeginStruct
	Float	tmsFreq			\ # of cycles per second
	Float	tmsTime0		\ When the first cycle started (for setting phase)
	Float	tmsAmplitude	\ Max magnitude of sine wave
	Float	tmsBias			\ "DC offset" added to sine wave
Struct timeSinerData

: TimeSiner	( [float] freq amplitude bias time0 -- )
    create					
	here >r					\ Store base addr on r stack	
	[SizeOf] timeSinerData
	allot					\ Allocate timeSinerData struct
	r@ tmsTime0 f!			\ Store Time0 in struct
	r@ tmsBias f!			\ Store Bias in struct
	r@ tmsAmplitude f!		\ Store Amplitude in struct
	r> tmsFreq f!			\ Store Frequency in struct
    does>					( -- baseaddr                        )
	fGetTime				( -- baseaddr    [float] -- time     )	
	dup tmsTime0 f@ f-		( -- baseaddr    [float] -- reltime  )
	dup tmsFreq f@ f*		( -- baseaddr    [float] -- freqtime )
	fsin					( -- baseaddr    [float] -- sineval  )
	dup tmsAmplitude f@ f*	( -- baseaddr    [float] -- loudval  )
	tmsBias f@ f+			( -- baseaddr    [float] -- offsval  )
;

\ ***************************************************************************
\ The PickRecord struct

\ The Pick Record struct.  If you change this here, change it
\ in meme.h.
BeginStruct

	Float	prVX		\ Near vertex's coords in object space 
	Float	prVY
	Float	prVZ

	Float	prWX		\ Spot exactly under the pixel in world space
	Float	prWY
	Float	prWZ

	Ptr		prShape
	Ptr		prPolygon
	Int		prVindex
Struct PickRecords
\ **************************************************************************
\ This meme waits till a mouse down-to-up has happened, and returns the
\ addr of the Meme object whose shape is under the cursor (if any.)  Returns
\ 0 if none found.
: SelectObject		\ -- objaddr
	begin
		pause MouseB
	until
	begin
		pause MouseB 0=
	until
	MouseX MouseY
	sp@ [SizeOf] PickRecords - 16 - 	\ Use stack for the data structure
	dup >r PickObject
	if
		r> prShape @ WhichObject
	else
		r> drop 0
	then
;

