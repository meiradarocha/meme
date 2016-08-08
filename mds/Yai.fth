\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Is this called yai.fth because it's "yet another interpret"?

decimal

: convert ( l addr --- l' addr' )
   \ convert 32-bit number, leaving address of first unconverted character
   begin  1+  dup  >r     ( l addr' ) ( r: addr+1 )
      c@  base  @  digit  ( l digit )
   while  >r  base @ *  r>  +
          r>
   repeat  drop  r>
;

\ : literal?  ( str -- str false  | literal true )
\    >r r@ number?  if   ( l )
\       r> drop  true
\    else
\       drop r> false
\    then
\ ;

: do-literal  ( l -- ?? )
   state @  if
16\   dpl @ 0>=  if  swap compile (lit) ,  else  drop  then
      compile (lit) ,
   else
      dpl @ 0>=  if  ( l->n )  then
   then
;
: do-undefined   ( str -- )
   error-output
   count type ."  ?"
   state @  if
      restore-output  compile lose
   else
      restore-output  cr quit
   then
;
: "compile ( str -- )
   canonical find  dup  if  (  cfa -1   |   cfa 1 )
      do-defined            (   )
   else                     ( str 0 )
      drop  literal?  if    ( l )
         do-literal         ( ?? )
      else                  ( str )
         do-undefined       ( )
      then
   then
;

: more?  ( str -- str f )  dup c@  ;
: (interpret  (s -- )
   begin   ?stack
     bl word   ( str )
     more?     ( str f )
  while
     "compile
  repeat
  drop
;
defer interpret
' (interpret is interpret
