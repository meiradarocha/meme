\ Two-user demo.  Drive a head around over the network.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.
\ http://www.immersive.com

3500 Module: foo

only main also std.mlContext! also foo also definitions

BeginModule

variable rtask		\ The rendering task

fvariable		selfX
fvariable		selfZ
fvariable		selfYaw
fvariable		cursorX
fvariable		cursorY
fvariable		lastx
fvariable		lastz
fvariable		lastyaw

\ My head
variable		self
variable		eye

\ Other person's head
variable		other
variable		head
variable		headMat

\ Floor
variable		floor

\ The lights
variable		light1

\ The tasks
variable		postask
variable		rendtask
variable		drivetask

\ **************************************************************************
\ Message-related stuff

variable		herIP
variable		ReplyReceived

1234	constant	PosMsgNum

\ Structure of a message header
BeginStruct
	Int			mhSrcHostID
	Int			mhSrcModuleID
	Int			mhDestHostID
	Int			mhDestModuleID
	Int			mhMsgLen
	Lbl			mhMsgBody
Struct MsgHeader

\ Structure of a position message
BeginStruct
	Int			pmMsgNum
	Float		pmPosX
	Float		pmPosZ
	Float		pmPosYaw
Struct PosMsg

\ **************************************************************************
\ First-quadrant mouse routines.  Both return -1.0 to 1.0.

0.03e	fconstant	mouseThreshold

: nx	\ [float] -- mousex
	MouseX float WindowWidth float f/ 2e f* 1e f-
	fdup fabs mouseThreshold f<
	if 
		fdrop 0e
	else
		mouseThreshold fover f0< if f+ else f- then
	then
;

: ny	\ [float] -- mousey
	MouseY float WindowHeight float f/ 2e f* 1e fswap f-
	fdup fabs mouseThreshold f<
	if 
		fdrop 0e
	else
		mouseThreshold fover f0< if f+ else f- then
	then
;

\ **************************************************************************
\ Ask the user for the hostname of the other person.
: RemoveLF		\ addr --
	begin dup c@ dup 0= not swap 10 = not and while 1+ repeat 0 swap c!
;
: GetHerHostname	\ --
	100 alloc-mem
	begin
		cr ." Please type your partner's computer name: "
		dup 100 expect
		dup RemoveLF
		dup NetGetHostByName ?dup 0=
	while
		cr ." Sorry, that hostname seems to be no good. "
	repeat
	nip herIP !
;

\ **************************************************************************
\ Task that drives my head around with the mouse
: DriveLoop		\ --
	begin
		MouseB leftButton and
		if
			nx cursorX f!
			ny cursorY f!
			cursorX f@ 0.08e f* selfYaw f+!
			selfYaw f@ fsincos
			cursorY f@ f* fnegate selfZ f+!
			cursorY f@ f* selfX f+!
			selfX f@ self @ x!
			selfZ f@ self @ z!
			selfYaw f@ fnegate 180e f*  pi f/ self @ yaw!
		then
		pause
	again
;

\ **************************************************************************
\ Rendering task
: r begin render pause again ;

\ **************************************************************************
\ Task that sends position of my head
create msg SizeOf MsgHeader SizeOf PosMsg + allot

0.05e fconstant deltaDist
2e fconstant deltaAngle

: MyPosLoop	\ --
	begin
		self @ x@ lastx f@ f- fabs deltaDist f>
		self @ z@ lastz f@ f- fabs deltaDist f> or
		self @ yaw@ lastyaw f@ f- fabs deltaAngle f> or
		if
			0 msg mhSrcModuleID !
			herIP @ msg mhDestHostID !
			0 msg mhDestModuleID !
			[SizeOf] PosMsg [SizeOf] MsgHeader + msg mhMsgLen !
			PosMsgNum msg mhMsgBody !
			self @ x@ fdup lastx f! msg mhMsgBody pmPosX f!
			self @ z@ fdup lastz f! msg mhMsgBody pmPosZ f!
			self @ yaw@ fdup lastyaw f! msg mhMsgBody pmPosYaw f!
			msg SendMessage drop
		then
		pause
	again
;

\ **************************************************************************
\ Receiver meme
: UpdateHerPos	\ msgbodyaddr --
	dup pmPosX f@ other @ x!
	dup pmPosZ f@ other @ z!
	    pmPosYaw f@ other @ yaw!
;

: rcvr			\ msgaddr --
	dup mhSrcHostID @ herIP @ =
	if
		mhMsgBody						\ msgaddr -- msgbodyaddr
		dup @
		case
			PosMsgNum of
				UpdateHerPos			\ msgbodyaddr --
			endof
			dup of						\ default case
				." Unknown msg: " ?
			endof
		endcase
	else
		drop							\ drop msgaddr
	then
; TheReceiverMemeIs rcvr

\ **************************************************************************
\ Go meme
: go
	if
		GetHerHostname

		ModuleWorld lockedLink plainVisible c" floor.rwx" NewVisible floor !
		-1e0 floor @ y!

		ModuleWorld lockedLink directionalLight NewLight light1 !
		0.5e 0.5e 0.5e light1 @ LightColor

		ModuleWorld lockedLink NewImaginary self !
		self @ lockedLink monoCamera 0 0 WindowWidth WindowHeight	
		NewCamera eye !
		2.3e eye @ y!

		ModuleWorld lockedLink NewImaginary	other !
		other @ lockedLink plainVisible c" head.rwx" NewVisible	head !
		270e0 head @ yaw!  2e head @ y!

		NewMaterial headMat ! 
		smoothShaded headMat @ MaterialShading
		0.2e 0.8e 1e 1e headMat @ MaterialSurface
		1e 0.9e 0.9e headMat @ MaterialColor
		headMat @ head @ ShapeMaterial

		0.4e 0.3e 0.1e eye @ SetCameraBackColor

		['] r NewTask ?dup if dup rendtask ! wake then
		['] MyPosLoop NewTask ?dup if dup postask ! wake then
		['] DriveLoop NewTask ?dup if dup drivetask ! wake then
		moduleOrigin RegisterDeckModule

	else
		drivetask @ DestroyTask
		rendtask @ DestroyTask
		postask @ DestroyTask
		head @ DestroyObject
		other @ DestroyObject
		eye @ DestroyObject
		self @ DestroyObject
		light1 @ DestroyObject
		floor @ DestroyObject
	then
; TheGoMemeIs go

ModuleUsed

EndModule

only
main also definitions

' foo ModuleAddr p" getahead.mm" UnloadModuleToFile

forget foo
