\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Defining words to construct Forth interfaces to C subroutines.
\ This uses the "ccall" primitive in the kernel, which contains a
\ mini-interpreter which does argument type conversion based
\ on an argument type string.  The argument type string is constructed
\ for each call by an argument specification list which looks a lot like
\ a Forth stack diagram.
\
\ Defines:
\
\ ccall:      ( subroutine-adr -- ) ( Input Stream: name arg-spec )

decimal
only main also definitions

vocabulary ccalls
ccalls also definitions

\ Scan the argument specification list.  For each argument, add a character
\ to the argument specifier string.

\ Valid argument specifier characters are:
\ i	- int
\ l	- long
\ a	- address
\ s	- string
\ --	- separates arguments from results
\ }	- terminates the list
: {  ( -- )
   0
   begin  bl word  1+ c@  dup ascii - =  until
   drop    \ Get rid of the '-'
   \ Now the stack has the argument spec characters in reverse order
   \ Put the input argument specs characters in the parameter field
   begin  dup  while  c,  repeat  drop
   ascii - c,
   \ Now do the result spec characters
   begin  bl word 1+  c@  dup ascii } <>  while  c,  repeat
   0 c,
   drop align
   previous
;
main definitions
: ccall:  \ name  { args } ( subroutine-address -- )
   create , 0 here c!
   also ccalls
   does>  dup na1+ swap @ ccall
;

only main also definitions

