\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

: places  ( n -- )  #places !  ;
3 #places !

decimal

\ Interoperable floating-point values consist of a 32-bit signed mantissa
\ and a 32-bit signed exponent.  The mantissa is converted to float,
\ divided by 2**31, and multiplied by 10**exponent.
\
\ To convert from float to interoperable float (if), the log base 10
\ of the float is taken.  The ceiling of the log + 1, when used to divide the
\ float, always yields a number 0.1 <= x < 1.0.  This number x is converted
\ int and used as the mantissa.  The ceiling of the log is converted to
\ int and used as the exponent.

\ The number of bytes in an interoperable floating-point value
8 constant /if		\ Same size as a float

\ Compile an interoperable float into the dictionary
\ [float] n --
: if,	f>if , , ;

\ Fetch and store an interoperable float
: if@	2@ if>f ;
: if!	f>if rot 2! ;

: ifliteral
	state @
		if
		compile (ifliteral)
		here /if allot if!
		then
; immediate

: fsincos  ( real -- sin cos )  fdup fsin fswap fcos  ;

: fclear  ( -- )  begin fdepth while fdrop repeat ;
: f.  ( real -- )  fstring type  space  ;
: e.  ( real -- )  estring type  space  ;
: f.s  ( -- )
    fdepth  0<  dup  if  fclear  then  abort" Floating Point Stack Underflow"
    fdepth if
	fdepth  0  ?do  fdepth i - 1- fpick f.  loop
    else
	." F-Empty "
    then
;
\ : fliteral  ( real -- )
\   state @  if  compile (fliteral) fpop  l, l,  then
\ ; immediate
: fliteral  ( real -- )
   state @  if  compile (ifliteral) if, then
; immediate
: f#  \ string  ( -- real )
   bl word  fnumber  [compile] fliteral
; immediate
: fliteral?  ( str -- flag )
   dup fnumber?  if  fnumber [compile] fliteral  r> drop exit  then
   literal?
;

\ patch fliteral? literal? "compile

/l 2* constant /f

: f,  ( real -- )  here /f allot f!  ;
: fvariable  \ name  ( -- )
   create  0E0 f,
;
: ifvariable  \ name  ( -- )
   create  0E0 if,
;
: fconstant  \ name  ( real -- )
   create  f,  does> f@
;
: ifconstant  \ name  ( real -- )
   create  if,  does> if@
;

4294967296e fconstant 2^32
2147483648e fconstant 2^31


: falog  ( real1 -- real2 )  1E1 fswap f**  ;

\ : fix  ( real -- int )  .5E0 f+ int  ;
: fix	( real -- int )
	fdup f0<
	if
		-0.5e
	else
		0.5e
	then
	f+ int
;

: fmax  ( real1 real2 -- real3 )  fover fover  f<  if  fswap  then  fdrop  ;

: fmin  ( real1 real2 -- real3 )  fover fover  f>  if  fswap  then  fdrop  ;

: fbetween 	\ -- flag
		\ [float] n lower upper --
	frot	\ [float] -- lower upper n
	fswap	\ [float] -- lower n upper
	fover	\ [float] -- lower n upper n
	f>=	\ [float] -- lower n
		\ -- flag
	f<= and
;
Internal
fvariable sintheta
fvariable costheta
External
: rotate	\ [float] x y radians -- x' y'
	fdup fcos costheta f!   fsin sintheta f!
	fover costheta f@ f*   fover sintheta f@ f* f-
	frot sintheta f@ f*   frot costheta f@ f* f+ ;
Bury

\ Floating-point constants
3.141592653589793238462643383e0 fconstant pi
pi 2e0 f* fconstant twoPi

: d>r	( [float] degrees -- radians ) 180e f/ pi f* ;
: r>d	( [float] radians -- degrees ) 180e f* pi f/ ;
