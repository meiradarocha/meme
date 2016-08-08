\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Meme language extension top-level definitions.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

\ *****************************************************************************
\ This file contains the memes that are not needed inside a module.  The
\ bottom-level memes, which are needed by modules, are in memeutil.m
\ *****************************************************************************


\ Defining memes for virtual objects
: camera:	( parent linktype subtype x y width height -- )
    create NewCamera , does> @ ;

: light:	( parent linktype subtype -- )
    create NewLight , does> @ ;

: visible: ( parent linktype subtype filenameaddr filenamelen -- ) 
    drop create NewVisible , does> @ ;

: sprite:	( parent linktype textureptr -- )
	create NewSprite , does> @ ;

( : audible: ... )

: imaginary:	( parent linktype -- )
    create NewImaginary , does> @ ;

\ Now in module.m
\ : compound:	( parent linktype filenameaddr filenamelen -- )
\    drop create (NewCompound) , does> @ ;

\
\ Defining meme for textures
: texture:	( filenameaddr filenamelen -- )
    drop create NewTexture , does> @ ;

\
\ Defining meme for materials
: material:	( -- )
   create NewMaterial , does> @ ;


\ *****************************************************************************
\ Some utilities for printing the objects and the hierarchy


Internal	\ *********************************************

: ObjNum	\ addr --
	?dup if
		world - [SizeOf] VirtualObjects /mod 	\ -- rem quot
		swap if ." in " then ." obj" dup 0 u.r
		ascii : emit object 0 u.r
\	else
\		." null ptr "
	then
;

: SiblingsNum	\ addr --
	begin
		?dup
	while
		dup ObjNum 2 spaces sibling @
	repeat
;

: arrows 0 ?do ." ->  " loop ;

: (ShowHier)	[ reveal ]  \ depth addr --
	begin
		cr over arrows dup ObjNum
		dup child @ if
			2dup child @ swap 1+ swap (ShowHier)
		then
		sibling @ ?dup 0=
	until
	drop
;

External	\ *********************************************

: ShowObj	\ addr --
	cr cr dup ObjNum
	cr ." &backptr " dup backptr . 
	."  backptr " dup backptr @ dup . ObjNum
	cr ." &child " dup child .
	."  child " dup child @ dup . ObjNum
	cr ." &sibling " dup sibling .
	."  sibling " dup sibling @ dup . ObjNum
	cr ." &parent " dup parent .
	."  parent " dup parent @ dup . ObjNum
	cr ." Children of " dup ObjNum ." ... "
	dup child @ SiblingsNum
	cr ." siblings of " dup ObjNum ." ... "
	sibling @ SiblingsNum
;

: ShowUsed
	world
	maxobjects 0 do
		dup used @ if
			dup ShowObj
		then
		[SizeOf] VirtualObjects +
	loop
    drop cr
;

		
: ShowHier
	0 world (ShowHier)
;

: .object	( objaddr -- )
	pause cr                      dup ObjNum
	pause cr ." used            " dup used ?
	pause cr ." struct ptr:     " dup ptr ?
	pause cr ." position:       " dup x f? dup y f? dup z f?
	pause cr ." orientation:    " dup yaw f? dup pitch f? dup roll f?
	pause cr ." scale:          " dup xscale f? dup yscale f? dup zscale f?
	pause cr ." child:          " dup child ?
	pause cr ." sibling:        " dup sibling ?
	pause cr ." backptr:        " dup backptr ?
	pause cr ." parent:         " dup parent ?
	pause cr ." object type:    " dup objecttype ?
	pause cr ." object subtype: " dup objectsubtype ?
	pause cr ." link type:      " linktype ?
;

Bury		\ *********************************************
\ ****************************************************************************
\ For timing of the animation
variable timingFrames		\ Start frame #
variable timingSeconds		\ Start time seconds
variable timingMilliseconds	\ Start time milliseconds
: StartTiming	( -- )
	GetTime timingMilliseconds ! timingSeconds ! 
	#frames @ timingFrames !
;

: FrameRate	( -- )
	GetTime
	float timingMilliseconds @ float f- 1000e f/
	float timingSeconds @ float f-
	f+
	#frames @ timingFrames @ - float
	fswap fdup f0=
		if
		." FrameRate: timing interval not long enough. "
		fdrop fdrop exit
		then
	f/ f. ." frames per second. "
;
\ ***************************************************************************
: .Doc	( -- )
   ." camera:    ( parent linktype subtype x y width height -- )"
cr ." light:     ( parent linktype subtype -- )"
cr ." visible:   ( parent linktype subtype filenameaddr filenamelen -- )"
cr ." sprite:    ( parent linktype textureptr -- )"
cr ." imaginary: ( parent linktype -- )"
cr ;
\ ***************************************************************************
\ Convenience memes for ginning up a world real quick
( -- )
: Eye: world lockedLink monoCamera 0 0 WindowWidth WindowHeight camera: ; 

( -- )
: Lite: world lockedLink pointLight light: ;

( filenameaddr filenamelen -- )
: Shape: >r >r world lockedLink plainVisible r> r> visible: ;

( filenameaddr filenamelen -- )
: Room: >r >r world lockedLink containerVisible r> r> visible: ;
