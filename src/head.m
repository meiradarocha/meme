\ Enigmatic head
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.
\ http://www.immersive.com

1000 Module: foo
main also
std.mlContext! also
foo also definitions
BeginModule

\ Variables that store the Meme-object pointers
variable eye		\ The eyepoint
variable lparent	\ The lights' parent
variable lite1		\ A light
variable lite2		\ A light
variable head		\ A human head
variable floor		\ A colored checkerboard floor	

\ Variables that store the task pointers
variable rtask		\ The rendering task
variable spintask	\ The light-spinning task

\ The background rendering loop
: r ( -- )
	begin 
		render		\ Draw a frame
		pause 		\ Switch context
	again			\ Forever
;

\ The background loop that spins the lights
: spin ( -- )
	begin 
		5e0 lparent @ yaw+! 	\ Increment the yaw angle of the lights' parent
		pause					\ Switch context
	again 
;

\
\ The "go" meme.  This meme runs to initialize and deinitialize the module
\
: go
	if	\ If loading

		\ Create the camera
		world lockedLink monoCamera
		0 0 WindowWidth WindowHeight NewCamera eye !

		\ Position it at (0, 0, 8)
		8e0 eye @ z!

		\ Set the background color
		0.4e 0.2e 0e eye @ SetCameraBackColor

		\ Create a parent node for the lights and position it at (0, 2, 0)
		world lockedLink NewImaginary lparent !
		2.5e0 lparent @ y!

		\ Create two lights
		lparent @ lockedLink pointLight NewLight lite1 !
		lparent @ lockedLink pointLight NewLight lite2 !

		\ Position the lights at (0, 0, 2) and (0, 0, -2) with respect
		\ to lparent's center.
		2.0e0 lite1 @ z! 	-2.0e0 lite2 @ z!

		\ Create the head and the floor
		world lockedLink plainVisible c" head.rwx" NewVisible head !
		90e0 head @ yaw!	\ Look left
		world lockedLink plainVisible c" floor.rwx" NewVisible floor !
		-2e0 floor @ y!		\ Move the floor to (0, -2, 0)

		\ Start the rendering task, saving its addr in rtask
		['] r NewTask ?dup if dup rtask ! wake then

		\ Start the spinning task, saving its addr in spintask
		['] spin NewTask ?dup if dup spintask ! wake then

	else	\ If unloading

		\ Kill the background tasks
		rtask @ DestroyTask
		spintask @ DestroyTask

		\ Destroy the Meme objects
		floor @ DestroyObject
		head @ DestroyObject
		lite1 @ DestroyObject
		lite2 @ DestroyObject

		\ Before destroying the camera, clear the graphics window
		0e 0e 0e eye @ SetCameraBackColor render	\ Clear graphics window
		eye @ DestroyObject

	then
; TheGoMemeIs go

ModuleUsed

EndModule

only
main also definitions

' foo ModuleAddr p" head.mm" UnloadModuleToFile

forget foo
