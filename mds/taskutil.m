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

: bg ' SpawnTask ;

\ ************************************************************************
\ The following code prints the names of all the available tasks.

\ Given the task adr, print the meme name of the task
: .Task	( adr -- )
	." UP =" dup 12 u.r space
	dup task-active? local @
	if
		." Awake " else ." Asleep" 
	then
	space space
	dup maintask = if				\ if the main task
		." Main Task "
	else
		dup up@ = if				\ If the current (but not main) task
			." Current Task "
		else						\ If this is not the main task
			dup named-task? local @
			if
				dup body> >name		\ Compute adr of name field
				.id					\ Print the name
			else
				." - unnamed -"	
			then
			3 spaces
		then
	then
	dup up@ <> if					\ If not the current task

		\ Print the parameter stack depth
		dup sp0 local @ 
		over saved-sp local @ -
		/n / ?dup if
			." Stk depth=" .
		else
			." Stk empty "
		then

		\ Print the return stack depth
		dup rp0 local @ 
		over saved-rp local @ -
		/n / ?dup if
			."    Rstk depth=" .
		else
			."    Rstk empty "
		then

	then
	drop	\ Drop the task address
;

\ Print the names of all tasks
: .Tasks	( -- )
	up@
	begin				\ For each task in linked list
		dup .Task cr	\ Print name ( -- adr )
		link local @	\ Follow link ( adr1 -- adr2 )
		dup up@ = 		\ Check if back to current task
	until				\ Loop if not
	drop				\ Drop task adr
;
