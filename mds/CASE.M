\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Case control structure for Meme.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

\ Now defined as a primitive
\ : (of)	\ n1 n2 -- n1 [if n1 <> n2]
\ 			\ n1 n2 --    [if n1 == n2]
\ 	over -
\ 	if
\ 		r> dup @ /n * + >r
\ 	else
\ 		drop
\ 		r> /n + >r
\ 	then
\ ;

\ : ($of)	\ n1 n2 -- n1 [if n1 <> n2]
\ 		\ n1 n2 --    [if n1 == n2]
\ 	over strcmp
\ 	if
\ 		r> dup @ /n * + >r
\ 	else
\ 		drop
\ 		r> /n + >r
\ 	then
\ ;

: ?cond	\ tag tag --
	<> abort" Conditionals not paired "
;

: of	\ -- addr tag
	4 ?pairs
	compile (of) >mark
	5
; immediate

: $of	\ -- addr tag
	4 ?pairs
	compile ($of) >mark
	5
; immediate

: endof	\ addr tag --
	5 ?pairs
	compile branch >mark swap >resolve
	4
; immediate

: case \ -- csp tag
	?comp  csp @ !csp  4
; immediate

: endcase	\ tag addr tag ... --
	4 ?pairs
	compile drop
	begin
		sp@ csp @ <>
	while
		>resolve
	repeat
	csp !
; immediate
