\ Spinning ball demo
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.
\ http://www.immersive.com

900 Module: foo

only main also
std.mlContext! also
foo also definitions
BeginModule

variable eye			\ Pointer to Meme camera object
variable ball			\ Pointer to Meme visible object
variable ballMat		\ Pointer to Meme material
variable lite			\ Pointer to Meme light object
variable rtask			\ Pointer to the rendering task
variable spintask		\ Pointer to the spinning task

\ The rendering background loop
: r 
	begin 
		render pause 
	again
;

\ The spinning background loop
: spin 
	begin 
		1e ball @ yaw+! pause 
	again 
;

\ The "go" meme.  Initializes and deinitializes the module.  Called
\ automatically when the module is loaded or unloaded.
: go

	if	\ If module is initializing

		\ Create the camera
		ModuleWorld lockedLink monoCamera
		0 0 WindowWidth WindowHeight NewCamera eye !
		eye @ 0e 0.6e 0.6e SetCameraBackColor
		10e eye @ z!

		\ Create the ball
		ModuleWorld lockedLink plainVisible 
		c" sphere.rwx" NewVisible ball !

		\ Create the material
		NewMaterial ballMat !
		0.4e 0.5e 0.5e 0.5e ballMat @ MaterialSurface
		1e 0e 0e ballMat @ MaterialColor
		ballMat @ ball @ ShapeMaterial

		\ Create the light
		ModuleWorld lockedLink pointLight NewLight lite !
		2.7e lite @ y!  2.8e lite @ z!

		\ Create the tasks
		['] r NewTask ?dup if dup rtask ! wake then
		['] spin NewTask ?dup if dup spintask ! wake then

	else	\ If module is deinitializing

		\ Destroy the tasks
		spintask @ ?dup if DestroyTask then
		rtask @ ?dup if DestroyTask then

		\ Destroy the objects and material
		ball @ DestroyObject
		lite @ DestroyObject
		ballMat @ DestroyMaterial

		\ Before destroying the camera, paint the graphics window black
		0e 0e 0e eye @ SetCameraBackColor render
		eye @ DestroyObject

	then

; TheGoMemeIs go

ModuleUsed
EndModule
only main also definitions

' foo ModuleAddr p" spinball.mm" UnloadModuleToFile
forget foo

