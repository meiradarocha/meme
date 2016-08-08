\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ quick and dirty decompiler

variable ip

: ip++		/n ip +! ;
: ipbranch	ip @ @ ip +! ;

: disp		\ --
	cr ." IP=" ip @ 0 u.r
	9 emit
	." Token=" ip @ @ 0 u.r
	9 emit
	ip @ @ dup maxprimitive <=
	if
		/n* origin + @
	then
	>name .id
;

: disp#		\ --
	cr ." IP=" ip @ 0 u.r
	9 emit
	." Value=" ip @ @
	base @ >r hex 0 .r r> base !
;

: toupper dup ascii a ascii z between if 32 - then ;

: wait-key	\ -- key
	key toupper
;


: dcmp	\ addr --
	ip ! disp
	begin
		wait-key
		case
			ascii D 	of ip++ ip++ disp			endof
			ascii Q		of abort					endof
			ascii N		of ip++ disp# 				endof
			ascii B		of ipbranch disp 			endof
			dup			of ip++ disp				endof
		endcase
	again
;
