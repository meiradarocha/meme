\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ New multitasking code for Meme.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

mod\ : local ( task-base user-var-addr -- user-var-addr-in-tasks-space )
mod\    up@ - +
mod\ ;

: sleep	( task-addr -- )	task-active? local off ;
: wake	( task-addr -- )	task-active? local on ;
: stop	( -- )				up@ sleep pause ;

\ Leave standard stack size and dict size values on stack.
: std-task-size	\ -- sp-#elems fp-#elems rp-#elems dict-bytes
	64 32 64 256
;

: compute-task-size	\ sp-#elems fp-#elems rp-#elems dict-bytes -- 
					\ sp-#elems fp-#elems rp-#elems dict-bytes total-bytes
	3 pick /n*						\ # bytes for int stack
	3 pick /f * +					\ Add # bytes for float stack
	2 pick /n* + 256 +				\ Add # bytes for r stack and tib
	over +							\ Add # dictionary bytes
	user-size +						\ Add # user area bytes
;

\ Allocate space for a task on the heap.  Return the task address.  Return
\ 0 if can't allocate.
: allocate-heap-task	\ sp-#elems fp-#elems rp-#elems dict-bytes -- 
						\ sp-#elems fp-#elems rp-#elems dict-bytes task-addr
	compute-task-size
	alloc-mem
;

: initialize-task	\ sp-#elems fp-#elems rp-#elems dict-bytes task-addr --

\ *********************************************
\ Following code doesn't work if run from code in a receiver meme.
\ The receiver meme is executed in a temporary context that is not
\ in the linked list of tasks.
\	\ Copy my user area to her user area
\	up@ over #user @ cmove
\
\	\ Her link already points to my successor.  Set my link to point to
\	\ to her.
\	dup link !

	\ Copy main task's user area to her user area
	maintask over					( task-addr -- task-addr maintask-addr task-addr )
	#user @							( -- #user )
	cmove							( task-addr maintask-addr task-addr #user -- task-addr )

	\ Her link already points to maintask's successor.  Set maintask's
	\ link to point to her.
	dup								( -- task-addr task-addr )
	maintask link local				( -- task-addr task-addr maintask-link-addr )
	!								( -- task-addr )

	\ Save my user pointer on r stk, set her user pointer
	up@ >r up!						( -- sp-# fp-# rp-# dict-bytes )

	\ Make sure her link has the relocation bit set
	link set-relocation-bit drop

	\ Save task size in user variable
	compute-task-size 				( -- sp-# fp-# rp-# dict-bytes t-size )

	\ Set top of return stack
	up@ + dup rp0 set-relocation-bit !	( -- sp-# fp-# rp-# dict-bytes rs-top )

	\ Set TIB and top of parameter stack
	2 pick /n* 256 + - 
\	dup tib set-relocation-bit !	*** Error!
	dup 'tib set-relocation-bit !
	dup sp0 set-relocation-bit !		( -- sp-# fp-# rp-# dict-bytes is-top )

	\ Set top of float stack
	4 pick /n* -
	dup fp0 set-relocation-bit !
	
	\ Set start of dictionary
	3 pick /f * - swap -
	dp set-relocation-bit !				( -- sp-# fp-# rp-# )

	drop drop drop						( -- )

	\ Set user pointer
	up@ up0 set-relocation-bit !		( -- )

	\ Clear the per-task data pointer
	pertask off							( -- )

	\ Clear the region DB state pointer
	region-var off						( -- )

	\ Put new task to sleep
	up@ sleep							( -- )

	\ Restore my up
	r> up!								( -- )
;


\ Find the address of the task link that points to task-addr
: find-task-link	( task-addr -- link-addr )

\ Can't use up@ from receiver meme--context not in linked list of
\ tasks.  Use maintask instead.
\	up@
	maintask

	begin
		link local dup @	\ -- task-addr link-addr link-contents
		2 pick <>			\ -- task-addr link-addr boolean
	while
		@					\ -- task-addr next-task-addr
	repeat
	nip						\ -- link-addr
;

\ Unlink the task from the circularly linked list.
\ NOTE! This code will fail if the task has not already been linked.
: unlink-task		\ task-addr --
	dup						\ -- task-addr task-addr
	dup link local @ swap	\ -- task-addr successor task-addr
	find-task-link			\ -- task-addr successor link-addr
	set-relocation-bit !	\ -- task-addr
	dup link local !		\ Link task to self just in case (like if
							\ the debugger runs off the end of the vectored
							\ meme.)
;

\ ****************************************************************************
\ The following code is for tasks allocated in the dictionary
\ ****************************************************************************

: allocate-dict-task	\ sp-#elems fp-#elems rp-#elems dict-bytes -- 
						\ sp-#elems fp-#elems rp-#elems dict-bytes task-addr
	compute-task-size
	here swap allot align
;

\ Long-jump to the parameter field of a colon definition
: longjmp >r ;

\ Wrapper code for tasks allocated in dictionary
: dict-task-wrapper	( ip -- )

	\ Initialize the vocabulary
main\	only main also
mod\	main

	\ Long-jump to the address on the int stack
	longjmp

	\ Make my task sleep; sleep again if awoken.
	begin
		stop
	again
;

\ Set the task's execution vector
: vector-dict-task	\ ip task-addr --
	up@ -rot   up!		( my-up cfa )


	\ Push the initial IP on the task's int stack
	sp0 @ /n - dup					\ -- my-up ip sp sp
	saved-sp set-relocation-bit !	\ -- my-up ip sp
	set-relocation-bit !			\ -- my-up

	\ Initialize ip for the task
	['] dict-task-wrapper >body
			saved-ip set-relocation-bit !	( my-up )

	\ Initialize fp, rp, frp  for the task
	fp0 @	saved-fp set-relocation-bit !	( my-up )
	rp0 @	saved-rp set-relocation-bit !	( my-up )
	0 		saved-frp set-relocation-bit !	( my-up )
	up!
;

\ Define a background task.
: background
	create
	std-task-size 			\ -- sp-# fp-# rp-# dict-bytes
	allocate-dict-task		\ -- sp-# fp-# rp-# dict-bytes task-adr
	dup >r 					\ -- sp-# fp-# rp-# dict-bytes task-adr
	initialize-task			\ -- 
	here r@					\ -- ip task-adr
	vector-dict-task		\ --
	r> named-task? local on	\ --
	!csp ]
	does>
;

\ ****************************************************************************
\ The following code is for tasks allocated on the heap
\ ****************************************************************************

\ Wrapper code for tasks allocated on the heap
: heap-task-wrapper	( cfa -- )

	\ Initialize the vocabulary
main\	only main also
mod\	main

	\ Execute the meme whose CFA has been placed on the stack.
	execute

	\ Unlink the task and free the associated memory.
	up@ dup unlink-task free-mem

	\ Enter the main task; don't try and follow the link anymore
	>maintask
;


\ Set the task's execution vector
: vector-heap-task	\ cfa task-addr --
	up@ -rot   up!		( my-up cfa )


	\ Push the CFA on the task's int stack
	sp0 @ /n - dup					\ -- my-up cfa sp sp
	saved-sp set-relocation-bit !	\ -- my-up cfa sp
	!								\ -- my-up

	\ Initialize ip for the task
	['] heap-task-wrapper >body
			saved-ip set-relocation-bit !	( my-up )

	\ Initialize fp, rp, frp for the task
	fp0 @	saved-fp set-relocation-bit !	( my-up )
	rp0 @	saved-rp set-relocation-bit !	( my-up )
	0 		saved-frp set-relocation-bit !	( my-up )
	up!
;

\ Allocate a new task on the heap, initialize its user area, and
\ set it up to run a meme.  Don't wake the task.
: NewTask	( cfa -- task-adr )
	std-task-size 			\ -- cfa sp-# fp-# rp-# dict-bytes
	allocate-heap-task		\ -- cfa sp-# fp-# rp-# dict-bytes task-adr
	dup
	if
		dup >r initialize-task	\ -- cfa
		r@ vector-heap-task		\ --
		r>						\ -- task-adr
	else
		nip nip nip nip nip		\ -- task-adr
	then
;

\ Put the task to sleep, unlink it, and free its heap space.
: DestroyTask ( task-adr -- )
	dup sleep
	dup unlink-task
	free-mem
;

\ Spawn a task running the cfa.  When cfa exits, unlink and deallocate
\ the task.
\ FIX: Have this meme set errno to indicate success or failure.
: SpawnTask ( cfa -- )
	NewTask ?dup if wake then
;

