\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Implementation of ANS X3J14 Basis 13 Local Variables.

variable to?  to? off
: to  ( -- )  to? on  ; immediate

variable #ins	\ Number of integer input variables
: do-local  ( local# -- )
   [compile] literal
   to? @  if  compile set-local  else  compile get-local  then
   to? off
;

variable #fins	\ Number of float input variables
: do-flocal ( local# -- )
	[compile] literal
	to? @ if compile set-flocal else compile get-flocal then
	to? off
;

: ?end-local-frame		#ins @ #fins @ + if compile free-locals then ;
: end-local-names		#ins off #fins off free-local-names ;
: ?end-locals			?end-local-frame end-local-names ;


\ Why is this code redefining exit ?  If exit runs ?end-locals, the
\ local variable names are cleared and can no longer be used after the
\ occurrence of exit.
\ I commented it out. -MdG
\
\ the "75" below is the primitive number for semicolon
\ the "85" below is the primitive number for does>
\ the "51" below is the primitive number for exit
\ If I use the definition names, the code fails when loaded into a module,
\ since ; and does> in the main dictionary have already been redefined
\ (by this code) as high-level definitions.
: ;      ?end-locals	[ 75 , ]			; immediate
: exit   ?end-local-frame  compile [ 51 , ] ; immediate
: does>  ?end-locals	[ 85 , ]			; immediate

\ This is the word defined in Basis
: (local)  ( adr len -- )
   dup  if
      #ins @ ['] do-local local-name  1 #ins +!
   else
		2drop
		#fins @ [compile] literal
		#ins @ [compile] literal   compile allocate-locals
   then
;

\ My addition -MdG
: (flocal)  ( adr len -- )
	dup  
	if
		#fins @ #ins @ + ['] do-flocal local-name  2 #fins +!
	else
		2drop
		#fins @ [compile] literal 
		#ins @ [compile] literal   
		compile allocate-locals
	then
;

: -end-locals  ( -- )
	#fins @ negate [compile] literal
   #ins @ negate [compile] literal   compile allocate-locals
;

\ Greg's syntax

\ : local bl word count (local)  ; immediate
\ : end-locals  0 0 (local)  ; immediate
\ : example ( n -- n n^2 n^3 )  local n end-locals n dup n * dup n * ;

\ Creative Solution's Syntax

: locals|
   begin
      bl word count  over c@  ascii | -  over 1 - or
   while
      (local)
   repeat
   2drop  0 0 (local)
; immediate
\ : x1  locals| n |  n  dup n *  dup n *  ;


\ Bradley Forthware's Syntax, with "ins" and "locals", but not "outs"

variable ;seen?
variable -seen?
variable |seen?
: {		\ --
	-seen? off  ;seen? off							\ --
	|seen? off
	begin
		bl word count								\ -- addr len
		over c@  									\ -- addr len char
		case
			ascii }  of             2drop true   endof
			ascii ;  of  ;seen? on  2drop false  endof
			ascii -  of  -seen? on  2drop false  endof
			ascii |  of  |seen? on ;seen? off -seen? off 2drop false endof
				over								\ -- addr len char len
				if
					false							\ -- addr len char false
					swap 							\ -- addr len false char
					2swap							\ -- false char adr len
					-seen? @
					if								\ output variable
						2drop						\ -- false char
					else  
						|seen? @
						if							\ float variable
							;seen? @
							if
								compile fzero
							then
							(flocal)
						else
							;seen? @  
							if						\ scratch variable
								compile false		\ -- false char addr len
							then
							(local)					\ -- false char
						then						( false char )
					then
				else
					nip nip true swap				\ -- true char
				then
		 endcase									( }-seen? )
	until											( )
	-end-locals
; immediate

\ : x2  { n -- }  n  dup n *  dup n *  ;
