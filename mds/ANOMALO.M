\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Yet another anomaloscope.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.
\

Internal		\ Start definitions internal to the debugger.

fload dbgdata.mh

\ *************************************************************************
\ The following code is data structures, variables and constants for
\ the anomaloscope.

: IP		debugger-data	_IP ;			\ The anomaloscope's IP

\ This is used by ui to determine when debug should stop debugging.  Sdebug
\ sets this to an empty stack pointer to allow debugging past the end of
\ the meme under test.
: starting-rp	debugger-data	_starting-rp ;		\ R stk lvl at dbg strt

\ Used by idle and the debugger to handshake.
: debug-in-progress	debugger-data	_debug-in-progress ; \ Flag for "idle"

\ The following is used for debugging non-high-level definitions.
\ "debug dup" will use this buffer to store the dup and run it.
: non-colon-def	debugger-data	_non-colon-def ;	\ Buff w/ 2 acf's

\ The anomaloscope's private return stack
: prp		debugger-data	_prp ;			\ Private r stk ptr
: prp0		debugger-data	_prp0 ;			\ Top of private r stk

\ Variables and constants
' memes @ constant docolon	\ The token for docolon
' (see) @ constant dodefer	\ The token for dodefer
/n negate constant -/n		\ Negative cellsize
variable 'debug-this		\ The acf of the recursion entry point
' crash 'debug-this set-relocation-bit ! 	\ Make sure we get a msg. 
						\ if uninitialized
\ CFAs of entry points into the debugger
variable '(debug)	' crash '(debug) token!
variable 'debug>	' crash 'debug> token!
variable '(sdebug)	' crash '(sdebug) token!
variable 'sdebug>	' crash 'sdebug> token!

\ *************************************************************************
\ The following code is for manipulating the private return stack

\ Clear the private return stack
: prp!	\ --
	prp0 prp !
;
prp!

\ Push and pop the private return stack
: >pr	\ n --
	-/n prp +!		\ Pre-decrement
	prp @ !			\ Store to top of private stack
;

: pr>	\ -- n
	prp @ @			\ Get top of private stack
	/n prp +!		\ Post-increment
;

\ Redefine : and ; to store return addresses on the private return stack.
: a:
	create hide docolon here /n - !
	compile r> compile >pr
	]
;

: a;
	compile pr> compile >r
\ FIX: Can't use the line below cuz locals.fth redefines ; so have
\ to comma the primitive number for ;
\	[compile] ;
	[ 75 , ]
; immediate

\ *************************************************************************
\ The following code is various utility memes.

\ Convert a character to upper-case
a: upc  ( char -- char' )
   dup ascii a ascii z between  if  ascii a -  ascii A +  then
a;

\ Set up the non-colon-def buffer
a: init-ncd-buff	\ --
    (lit) unnest
    dup				\ -- token token
    non-colon-def dup >r !	\ -- token
    r> /n + !			\ --
a;

\ *************************************************************************
\ The following code is the execution engine for the anomaloscope.

\ The various possible behaviors of IP for one execution step
a: default-nextIP	/n IP +! a;
a: branch-nextIP	default-nextIP IP @ @ /n* IP +! a;
a: skip1-nextIP		default-nextIP default-nextIP a;
\ We need two ways of handling strings because some strings are
\ null-terminated, and some are not. 
a: string-nextIP	default-nextIP IP @ count + 1+ aligned IP ! a;
a: stringlit-nextIP	default-nextIP IP @ count + aligned IP ! a;
a: float-nextIP		default-nextIP /f IP +! a;

\ Routines for simulating the behavior of various memes
a: do-abort	\ --
    debug-in-progress off abort	\ Exit from debugger
a;
a: do-(do)	\ limit start --
    IP @ /n + >r		\ Address of offset to end on r stk
    over >r			\ limit on r stack
    swap - >r			\ distance up to zero on r stack
    skip1-nextIP
a;
a: do-(loop)	\ --
    1 rp@ +!			\ Increment the distance up to zero
    rp@ @ 			\ Get the distance
	if			\ If not done with loop yet
	branch-nextIP		\ Branch back
	else			\ If done with loop
	r> drop r> drop r> drop \ Clear rtn stk of do arguments
	skip1-nextIP		\ Skip past loop branch offset
	then
a;
\ this is WRONG
a: do-(+loop)	\ n --
    rp@ +!			\ Increment the distance up to zero
    rp@ @ 			\ Get the distance
	if			\ If not done with loop yet
	branch-nextIP		\ Branch back
	else			\ If done with loop
	r> drop r> drop r> drop \ Clear rtn stk of do arguments
	skip1-nextIP		\ Skip past loop branch offset
	then
a;
a: do-of		\ n1 n2 -- [n1]
	over =
	if
		drop skip1-nextIP
	else
		branch-nextIP
	then
a;
a: do-$of		\ addr1 addr2 -- [addr1] 
	over strcmp 0=
	if
		drop skip1-nextIP
	else
		branch-nextIP
	then
a;
a: do-throw		\ err -- [err]
	dup							\ err -- err err
	if							\ err err -- err
		handler @ 				\ err -- err handler
		if						\ err handler -- err
			handler @ rp!		\ err -- err
			r> swap				\ err -- oldsp err
			>r sp!				\ oldsp err --		Save err on rstk cuz tos
			r>					\ -- err			is in a register
			r> IP !				\ err -- err
		else
			sp0 @ sp!
			rp0 @ rp!
			debug-in-progress off
			abort
		then
	then
a;
a: do-leave	\ --
    r> drop r> drop
    r> /n - IP !
    branch-nextIP
a;
a: do-(ifliteral)	\ [float] -- f
    IP @ /n + if@
    float-nextIP
a;
a: do-(debug)	\ cfa --
    IP @ /n + >r 'debug-this @ execute 
a;

\ The execution engine.  Assumes that IP points into a colon definition's
\ parameter field.  Executes one step.
a: step-once	\ --
    IP @ @		\ Get the next token or acf to be executed
	case

	\ Handle every primitive that manipulates IP
	(lit) branch	of branch-nextIP				endof
	(lit) ?branch	of 0= if branch-nextIP else skip1-nextIP then	endof
	(lit) unnest	of r> IP !					endof
	(lit) exit	of r> IP !					endof
	(lit) abort	of do-abort					endof
	(lit) (do)	of do-(do)					endof
	(lit) (?do)	of 2dup <> if do-(do) else skip1-nextIP then	endof
	(lit) leave	of do-leave					endof
	(lit) ?leave	of if do-leave then				endof
	(lit) (loop)	of do-(loop)					endof
	(lit) (+loop)	of do-(+loop)					endof
	(lit) (of)		of do-of						endof
	(lit) ($of)		of do-$of						endof
	(lit) throw		of do-throw						endof

	\ Handle every primitive with an in-line integer or float
	(lit) (lit)	of IP @ /n + @ skip1-nextIP						endof
	(lit) (fliteral) of IP @ /n + f@ float-nextIP				endof
	(lit) (ifliteral) of do-(ifliteral)							endof
	(lit) (')	of IP @ /n + token@ skip1-nextIP				endof

	\ Handle every primitive with an in-line string
	(lit) (.")	of IP @ /n + count type stringlit-nextIP		endof
	(lit) (abort")	of if do-abort else stringlit-nextIP then	endof
	(lit) ("s)	of IP @ /n + string-nextIP						endof
	(lit) (")	of IP @ /n + count string-nextIP				endof
	(lit) (c")	of IP @ /n + count drop string-nextIP			endof

	\ Handle re-entries into the debugger
	'debug> @	of default-nextIP				endof
	'(debug) @	of do-(debug)					endof
	'sdebug> @	of default-nextIP				endof
	'(sdebug) @	of do-(debug)					endof

	\ Handle everything else
	dup ( default ) of IP @ @ execute default-nextIP		endof
	endcase
a;

\ *************************************************************************
\ The following routines are for the user interface to the debugger.
a: show-IP	\ --
	cr IP @
	    case
	    non-colon-def
		of
		." -------------  " IP @ token@ >name .id
		endof
	    non-colon-def /n +
		of
		." -------------  <done> "
		endof
	    dup
		of
		." IP=" IP @ dup 9 u.r ." : " token@ >name .id
		IP @ @
		    case
			(lit) (lit)        of 	IP @ /n + @ .	    endof
			(lit) (fliteral)   of	IP @ /n + f@ f.	    endof
			(lit) (ifliteral)  of	IP @ /n + if@ f.    endof
			dup                of			    endof
		    endcase
		endof
	    endcase
a;

\ The anomaloscope needs its own version of .s (that doesn't call abort)
a: anom.s  ( -- )  
    depth 0< if
	." Under "
    else
	depth 0= if
	    ." Empty "
	else
	    (.s
	then
    then
a;

a: r.s		\ --
	rp0 @ rp@ <>
	if							\ If return stack not empty
		rp0 @					\ -- rp0
		begin					\ For each stack item
			/n - dup rp@ >= 	\ Decrement item ptr, compare w/ r stk ptr
		while 					\ While still items to print
			dup ?				\ itemptr -- itemptr
		repeat
		drop					\ itemptr --
	else						\ If return stack empty
		." R-Empty "
	then
a;

a: show-stacks	\ ---
	." ( " 
	anom.s  ." / " f.s ." / " r.s
	." ) "
a;

a: show-state	\ --
	show-IP show-stacks
a;

\ The D command.  Go down one level.
a: do-D	\ --
	\ If we are descending into execute, pass execute's
	\ calling argument to debug-this, else fetch the acf that
	\ IP is pointing at.  If the acf points at a : def or
	\ does> def, descend into it, otherwise treat it like
	\ the default case (just execute it).
	IP @ @ (lit) execute =			\ Check if acf is execute
	if								\ If it's execute
	    IP @ /n + >r				\ Push return address
	    'debug-this @ execute		\ Recurse into debug-this
	else
		IP @ @ maxprimitive <=
		if							\ If it's a primitive
			step-once				\ Just execute it
		else
		    IP @ @ @ dodefer =
		    IP @ @ @ docolon = or
		    IP @ @ @ maxcf > 
		    IP @ @ maxprimitive > and
		    or
		    IP @ @ @ 'debug> @ <>
		    IP @ @ @ '(debug) @ <> and
		    IP @ @ @ 'sdebug> @ <> and
		    IP @ @ @ '(sdebug) @ <> and
		    and
			if							\ If : def or does> def or deferred def
				IP @ /n + >r			\ Push return address
				IP @ @					\ Get the acf
				'debug-this @ execute	\ Recurse into debug-this
			else						\ If it's something else
				step-once				\ Just execute it
			then
		then
	then
a;

\ The U command.
\ Go up one level.  Execute at speed until the current
\ high-level definition has exited.
a: do-U
	r@					\ Get the return addr
    begin
	    >pr				\ Hide rtn addr so code under
						\ test sees only its own stk items
	    step-once		\ Single-step
	    pr>				\ Retrieve rtn addr
	    dup IP @ =		\ Compare with IP
    until				\ Loop until IP equals return addr
	drop
a;

\ The Esc command.  Exit.  Since the return stack only has 
\ return addresses to the code under test, a simple exit suffices to return
\ control to the code.  It runs to completion on its own.
a: do-Esc	\ --
	debug-in-progress off   IP @ >r   exit
a;

\ The Q command.  Quit.  Stop execution of the code under test
\ and immediately return to the interpreter.
a: do-Q		\ --
	debug-in-progress off   cr ." Aborted "   abort
a;

\ Invoke a Meme interpreter under the debugger.  Execute resume
\ to resume debugging.
a: do-M		\ --
	begin interpret-line again
a;

\ Default behavior.  Execute the meme pointed at by IP.
\ Set IP to the new address.
a: do-default	\ --
	step-once
a;

\ The user interface.  Show the current state of the code under test,
\ then wait for a key from the user.
a: ui	\ --
    begin
	
	\ Exit if the return stack has been popped past the level
	\ the debugger started at.
	rp@ starting-rp @ u> if do-Esc then
	show-state
	key upc
	    case
		ascii D		of do-D			endof
		ascii U		of do-U			endof
		27 ( Esc )	of do-Esc		endof
		ascii Q		of do-Q			endof
		ascii M		of do-M			endof
		dup		of do-default		endof
	    endcase
    again
a;
\ *************************************************************************
\  The outermost shell of the debugger.  Called recursively.
a: debug-this	( acf -- )	[ reveal ] \ Allow self-reference
    dup @ docolon = if		\ If it's a colon def
	cr ." : " dup >name .id	\ Print the name of the def
	/n + IP !		\ Point IP at the parameter field
	ui			\ Go do the user interface
    else
	dup @ dodefer = if
	    cr ." Deferred " dup >name .id ." is "
	    >body token@ debug-this
	else
	    dup @ maxcf > if	\ If the cf > max token #
			cr ." does> " 	\ It's a does> word
			dup @ IP !		\ Point IP at the code
			/n +			\ Leave the PFA on the stack
			ui				\ Go do the user interface
	    else				\ Otherwise, it's a primitive
							\ or constant or var or vocab etc.
			@ non-colon-def ! non-colon-def IP ! ui
	    then
	then
    then
a;
\ Resolve the forward reference (needed for recursion)
' debug-this 'debug-this set-relocation-bit !
\ *************************************************************************
\ This is the piece of code that "attach" forces the background task
\ to execute.
: (attach)
    debug-in-progress @ 0= if
		prp!				\ Initialize the private return stack
		init-ncd-buff		\ Initialize the non-colon-def buffer
		rp0 @ starting-rp !	\ Set the return stack level at debug start
    then
    1 debug-in-progress +!	\ Let others know we're debugging.
    ui
    -1 debug-in-progress +!
;

External		\ Start definitions visible outside the debugger

\ *************************************************************************
\ The following memes allow the debugger to be invoked in various ways.
\
\ (debug) and debug> are the lowest entry points to the debugger.
\ If (debug) is called from within existing code, e. g. "(lit) foo (debug)",
\ the entire debugger's context is absent from the return stack, and
\ the code under test sees a stack environment that is identical to
\ execution without the debugger.
\ When in the code, ": foo ['] xx (debug) yy ;"  (debug)'s return address
\ (returning to execute yy) is what xx would have returned to anyway.
\
\ debug> is inserted into code to start debugging in-line. Example:
\ : xyz 1 2 + debug> 3 * 4 / ;
\
: (debug)	\ acf --
    debug-in-progress @ 0= if
	prp!		\ Initialize the private return stack
	init-ncd-buff	\ Initialize the non-colon-def buffer
        rp@ starting-rp ! \ Set the return stack level at debug start
    then
    1 debug-in-progress +!	\ Let others know we're debugging.
    debug-this 			\ Go debug this meme
    -1 debug-in-progress +!
; ' (debug) '(debug) set-relocation-bit !

: debug>	\ --
    debug-in-progress @ 0= if
	prp!		\ Initialize the private return stack
	init-ncd-buff	\ Initialize the non-colon-def buffer
	rp@ /n + starting-rp !	\ Set the return stack level at debug start
    then
    1 debug-in-progress +!	\ Let others know we're debugging.
    r> IP ! ui		\ Go debug the next meme in-line
    -1 debug-in-progress +!
; ' debug> 'debug> set-relocation-bit !

\ Debug a meme whose name is passed as an argument
: "debug	\ pstr --
    find if (debug) else count type ." not found " then
;

\ Usage: debug memename
: debug	( -- ) bl word "debug ;

\ The "super" debugger is like the debugger, except it will step
\ past the unnest at the end of the debugged definition.
: (sdebug)	\ acf --
    debug-in-progress @ 0= if
	prp!		\ Initialize the private return stack
	init-ncd-buff	\ Initialize the non-colon-def buffer
        rp0 @ starting-rp ! \ Set the return stack level at debug start
    then
    1 debug-in-progress +!	\ Let others know we're debugging.
    debug-this			\ Go debug this meme
    -1 debug-in-progress +!
; ' (sdebug) '(sdebug) set-relocation-bit !

: sdebug>	\ --
    debug-in-progress @ 0= if
	prp!		\ Initialize the private return stack
	init-ncd-buff	\ Initialize the non-colon-def buffer
	rp0 @ starting-rp !	\ Set the return stack level at debug start
    then
    1 debug-in-progress +!	\ Let others know we're debugging.
    r> IP ! ui		\ Go debug the next meme in-line
    -1 debug-in-progress +!
; ' sdebug> 'sdebug> set-relocation-bit !

\ Debug a meme whose name is passed as an argument
: "sdebug	\ pstr --
    find if (sdebug) else count type ." not found " then
;

\ Usage: sdebug memename
: sdebug	( -- ) bl word "sdebug ;
\ ****************************************************************************
\ If the debugger is invoked from a background task, it can compete
\ with the main task for character I/O.  To avoid the problem, run idle
\ from the main task before the debugger is invoked.  When the debugger exits
\ it will set the flag that idle waits for.
: idle	\ --
	debug-in-progress @ 0=
	if
		begin
			pause pause pause pause
			debug-in-progress @
		until
	then
    begin
		pause pause pause pause
		debug-in-progress @ 0=
    until
;
\ ****************************************************************************
\ Attach to a running background process.
: attach	( taskaddr -- )
    dup up@ = abort" Can't attach to self "
	dup task-active? local @ 0= 
	if
		cr ." Task is asleep, waking it. " cr
		dup wake
	then
	dup saved-ip local @ IP !			\ Save the task's IP in the debugger
    ['] (attach) >body swap 
    saved-ip local !					\ Point the task's IP at the debugger
    idle								\ Idle the foreground task
;
\ ****************************************************************************
\ Resume debugging from the debugger's interpreter loop.
: resume
    debug-in-progress @ if
	r> drop			\ Drop my return addr to do-M
	pr> >r exit		\ Return to do-M's caller
    then
;

Bury			\ Bury the internal definitions
