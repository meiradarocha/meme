\ This world allows airplane-style flying with the mouse.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.
\ http://www.immersive.com
\ 
\ This is a virtual world with several animated objects in it.  The
\ human navigates the virtual world with the mouse, using it as an
\ airplane-style joystick for flying.  The human can fly right through
\ the checkerboard floor, which becomes a ceiling.
\ Pressing the right mouse button moves slowly.  Pressing the left
\ mouse button moves fast.

decimal

5900 Module: foo

only main also
std.mlContext! also
foo also definitions

BeginModule

\ Number of octahedrons bouncing around
12 constant #octos

variable head
variable headYaw
variable headPitchRoll
variable camera

variable floor
variable otherside

variable bouncebox

variable 4balls
variable ball1
variable ball2
variable ball3
variable ball4
variable tex
variable lite
variable star

variable BallMat
variable StarMat

variable ltt
variable 4bpt
variable 4byt
variable bt
variable rt
variable mt

4e ifconstant box

box fnegate ifconstant -box

\ The octo struct
BeginStruct
	Ptr		objptr
	Float	xTraj
	Float	yTraj
	Float	zTraj
	Float	yawSpin
	Float	pitchSpin
	Float	rollSpin
Struct Octos

\ Balls' distance from the center of rotation
1.2e0 ifconstant dist
dist fnegate ifconstant -dist

\ Generate random initial values for trajectory and spin

: TrajRand		( -- [float] n ) frand 0.5e f- .1e f* ;
: SpinRand		( -- [float] n ) frand 0.5e f- 10e f* ;
: ReSpinRand	( -- [float] n ) frand 0.5e f- 30e f* ;

\ Allocation and initialization of the octos

create octos	SizeOf Octos #octos * allot align

: InitOctos	( -- )
	octos	[SizeOf] Octos #octos *
	over + swap
	do
		bouncebox @ swivelLink plainVisible 
		c" octozip.rwx"
		NewVisible i objptr !
		TrajRand i xTraj f!
		TrajRand i yTraj f!
		TrajRand i zTraj f!
		SpinRand i yawSpin f!
		SpinRand i pitchSpin f!
		SpinRand i rollSpin f!
	[SizeOf] Octos
	+loop
;

\ This meme changes the spin on the octo.  Called after a bounce.
: ReSpin ( addr -- )
	ReSpinRand dup yawSpin f!
	ReSpinRand dup pitchSpin f!
	ReSpinRand     rollSpin f!
;

\ This is the animation behavior for the octahedrons.
: DoOctos	( -- )
	octos [SizeOf] Octos #octos *
	over + swap
	do

		i xTraj f@ 		i objptr @ x+!
		i yTraj f@ 		i objptr @ y+!
		i zTraj f@		i objptr @ z+!
		i yawSpin f@	i objptr @ yaw+!
		i pitchSpin f@ 	i objptr @ pitch+!
		i rollSpin f@ 	i objptr @ roll+!
		i objptr @ x@ fabs box f>
			if i xTraj f@ fnegate i xTraj f! i ReSpin then
		i objptr @ y@ fabs box f> 
			if i yTraj f@ fnegate i yTraj f! i ReSpin then
		i objptr @ z@ fabs box f> 
			if i zTraj f@ fnegate i zTraj f! i ReSpin then

	[SizeOf] Octos +loop
;

\ The move-the-little-light task
120e0 6e0 0e0 0e0 siner starsiner		\ FIX: siner uses native FP constants
: lttask begin starsiner star @ y! pause again ;

\ Slowly rotate the whole 4balls structure
: 4ballspitchtask begin 0.1e0 4balls @ pitch+! pause again ;

\ Quickly rotate the whole 4balls structure
: 4ballsyawtask begin 12e0 4balls @ yaw+! pause again ;

\ Bounce-the-octos task
: bouncetask begin DoOctos pause again ;

\ Rendering task
: r camera @ CurrentCamera begin render pause again ; 

: timer\ [compile] \ ; immediate
\ : timer\ ; immediate

\ Timer routines
timer\	fvariable tnow					\ Current time, set by now!
timer\	fvariable tthen					\ Last value of now, set by then!
timer\	: now! fGetTime tnow f! ;
timer\	: then! tnow f@ tthen f! ;
timer\	: delta@ tnow f@ tthen f@ f- ;

timer\	\ Multiply by the amount of time (in seconds) since "then"
timer\	: delta* ( [float] f1 -- f1*#seconds )
timer\		tthen f@ f0= if			\ If "then" uninitialized
timer\			0e					\ Multiply by zero
timer\		else
timer\			delta@				\ Multiply by time diff in secs
timer\		then
timer\		f*
timer\	;

fvariable lastts
TimeScaler ts

\ Convert degrees to radians
: d->r pi f* 180e f/ ;

\ Convert Mouse Y to +-30 degrees
: CvtMouseToPitch       ( -- angle )
	MouseY 60 * WindowHeight / 30 - ;

\ Control the pitch of the camera with the Y of the mouse
\ Move 
: SetPitchFromMouseY ( -- )
	CvtMouseToPitch float
	headPitchRoll @ pitch@ f- 3e f* lastts f@ f*
	headPitchRoll @ pitch+! ;

\ Convert Mouse X to +-30 degrees
: CvtMouseToRoll ( -- angle )
	30 MouseX 60 * WindowWidth / - ;

: SetRollFromMouseX ( -- )
	CvtMouseToRoll float
	headPitchRoll @ roll@ f- 3e f* lastts f@ f* 
	headPitchRoll @ roll+! ;

: SetYawFromRoll ( -- )
	headPitchRoll @ roll@ lastts f@ f* headYaw @ yaw+! ;

fvariable theSpeed      \ This is effectively the throttle setting
: speed theSpeed f@ lastts f@ f* ;
: -speed speed fnegate ;
: SetNewPosition ( -- )
	headPitchRoll @ pitch f@ d->r fsin speed f* head @ y+!
	headYaw @ yaw f@ d->r fsincos
	-speed f* head @ z+!
	-speed f* head @ x+! ;
	
\ Fly with the mouse
: FlyWithMouse ( [float] speed -- )
	1e ts fdup lastts f! f*
	theSpeed f!
	SetPitchFromMouseY	
	SetRollFromMouseX
	SetYawFromRoll
	SetNewPosition 
;

: FlySlowWithMouse 100e FlyWithMouse ;
: FlyFastWithMouse 1500e FlyWithMouse ;
: Don'tFlyWithMouse fGetTime ['] ts >body f! ;

: m begin DoMouse pause again ;

: go
	if		\ If loading the module

		world swivelLink NewImaginary head !
		head @ lockedLink NewImaginary headYaw !
		headYaw @ lockedLink NewImaginary headPitchRoll !
		35e head @ z!
		['] ts 0.05e InitTimeScaler

		headPitchRoll @ lockedLink 
		monoCamera
		\ stereoInterlacedRLCamera
		0 0 WindowWidth WindowHeight NewCamera
		camera !
		camera @ 0e 0e 0.5e SetCameraBackColor

		world swivelLink plainVisible 
		c" floor.rwx" NewVisible floor !
		world swivelLink plainVisible 
		c" floor.rwx" NewVisible otherside !
		-10e floor @ y! 0.25e floor @ rescale
		180e otherside @ pitch!
		-10e otherside @ y! 0.25e otherside @ rescale

		world swivelLink NewImaginary bouncebox !
		InitOctos

		\ This is the link point for the 4 balls and the star light
		world swivelLink NewImaginary 4balls !

		\ Define some shapes
		\ parent    linktype   subtype      filenameaddr
		  4balls @  lockedLink plainVisible	
			c" sphere.rwx"   NewVisible ball1 !
		  4balls @  lockedLink plainVisible	
			c" sphere.rwx"   NewVisible ball2 !
		  4balls @  lockedLink plainVisible	
			c" sphere.rwx"   NewVisible ball3 !
		  4balls @  lockedLink plainVisible	
			c" sphere.rwx"   NewVisible ball4 !
		  4balls @  lockedLink plainVisible	
			c" octozip.rwx"  NewVisible star !

		\ Scale the star to 3/10 its normal size
		0.3e0 star @ rescale

		\ Create the light that's attached to the star
		\ parent linktype   subtype
		  star @ lockedLink pointLight NewLight lite !

		\ Position the 4 balls
		-dist ball1 @ x! -dist ball1 @ z!
		-dist ball2 @ x! dist ball2 @ z!
		dist ball3 @ x! -dist ball3 @ z!
		dist ball4 @ x! dist ball4 @ z!

		\ Position the whole group
		-10e 4balls @ x!  -10e 4balls @ z!

		\ Set the attributes of the balls
		NewMaterial BallMat !
		1e 0e 0e BallMat @ MaterialColor
		1e BallMat @ MaterialOpacity
		0.5e 1e 0.5e 1e BallMat @ MaterialSurface
		solidGeometry BallMat @ MaterialGeometry
		smoothShaded BallMat @ MaterialShading
		BallMat @ ball1 @ ShapeMaterial
                0.5e BallMat @ MaterialOpacity
		BallMat @ ball2 @ ShapeMaterial
		flatShaded BallMat @ MaterialShading
                yellowColor BallMat @ MaterialColor
		BallMat @ ball3 @ ShapeMaterial
                1e BallMat @ MaterialOpacity
                \ The bmp file works as a texture when it's 128x128
                c" turf.bmp" NewTexture tex !
                tex @ BallMat @ MaterialTexture
		BallMat @ ball4 @ ShapeMaterial

		\ Make the star bright white
		NewMaterial StarMat !
		1e 1e 1e StarMat @ MaterialColor
		1e StarMat @ MaterialOpacity
		1e 1e 1e 1e StarMat @ MaterialSurface
		solidGeometry StarMat @ MaterialGeometry
		smoothShaded StarMat @ MaterialShading
		StarMat @ star @ ShapeMaterial

		['] lttask NewTask dup ltt ! wake 
		['] 4ballspitchtask NewTask dup 4bpt ! wake
		['] 4ballsyawtask NewTask dup 4byt ! wake
		['] bouncetask NewTask dup bt ! wake
		['] r NewTask dup rt ! wake
		['] m NewTask dup mt ! wake
		['] FlySlowWithMouse leftButton MouseVector!
		['] FlyFastWithMouse rightButton MouseVector!
		['] Don'tFlyWithMouse 0 MouseVector!

	else		\ If unloading the module

		mt @ DestroyTask
		rt @ DestroyTask
		bt @ DestroyTask
		4byt @ DestroyTask
		4bpt @ DestroyTask
		ltt @ DestroyTask
		tex @ DestroyTexture
		StarMat @ DestroyMaterial
		BallMat @ DestroyMaterial
		lite @ DestroyObject
		star @ DestroyObject
		ball4 @ DestroyObject
		ball3 @ DestroyObject
		ball2 @ DestroyObject
		ball1 @ DestroyObject
		4balls @ DestroyObject
		octos [SizeOf] Octos #octos * over + swap
		do
			i objptr @ DestroyObject
		[SizeOf] Octos +loop
		bouncebox @ DestroyObject
		otherside @ DestroyObject
		floor @ DestroyObject

		\ Clear the window
		camera @ 0e 0e 0e SetCameraBackColor  render

		camera @ DestroyObject
		headPitchRoll @ DestroyObject
		headYaw @ DestroyObject
		head @ DestroyObject
	then
;

TheGoMemeIs go		\ Mark "go" as the module's go meme

ModuleUsed

EndModule

only main also definitions

' foo ModuleAddr p" flying.mm" UnloadModuleToFile

forget foo
