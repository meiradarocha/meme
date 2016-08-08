\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Code to set the task pointers in a module

\ 
\ This meme returns true if the supplied address is in the current module's
\ dictionary space.  It will not return TRUE if the addr is the module
\ origin, which always has a zero in it, and is not an accessible part
\ of the dictionary.
: InsideModuleDict? 			( addr -- boolean					)
	ThisModule ModuleDict /n +	( -- addr.of.1st.dict.byte			)
	ThisModule RelTable 1-  	( -- 1st.dict.addr last.dict.addr	)
	between						( -- bool							)
;

: ?LinkedTask	{ task ; nexttask -- }			\ task -- bool
	up@ to nexttask
	begin
		task nexttask = if true exit then
		nexttask link local @ to nexttask
		nexttask up@ =
	until
	false
;
			
\ Unlink the task from the circular list
: UnlinkTask		{ taskaddr ; mytask nextlink -- }
	?LinkedTask 0= if exit then						\ If not linked, we're done
	taskaddr link local @ taskaddr = if exit then	\ if taskaddr points at itself, we're done
	mytask
	begin
		link local to nextlink
		nextlink @ mytask = if exit then
		nextlink @ taskaddr = if nextlink @ to before
	ink local @ taskaddr <> 





	up@ to mytask							\ My task addr
	link to nextlink						\ Start with me to walk the list
	begin
		nextlink @ taskaddr =
		if									\ If the link points at taskaddr
			taskaddr link local @ nextlink !	\ Point the link at who taskaddr points to
			exit								\ We're done
		then
		nextlink @ link local to nextlink		\ Skip to next link
		nextlink link =							
	until										\ Until next link is my link
; 
	
