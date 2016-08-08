\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Machine/implementation-dependent definitions
decimal

/n constant /branch

\ For null-terminated strings, i. e. (") ("s)
: +str    ( ip -- ip' )
  count + 1+ aligned		\ Modified by MdG
;
\ For non-null-terminated strings, i. e. (abort") and (.")
: +strlit ( ip -- ip' )
  count + aligned
;
: >data  ( acf -- data-adr )
   dup word-type               ( n acf code-field-word )
   dup ['] #user word-type  =  if  deferadr exit  then
   dup ['] defxx word-type  =  if  deferadr exit  then
   dup ['] main word-type  =  if  deferadr exit  then
   drop >body !
;

\ >target depends on the way that branches are compiled
: >target ( ip-of-branch-instruction -- target )
   ta1+ dup @ na+
;

\ Since ;code is not implemented yet, (does always introduces a
\ does> clause.
: does-ip?   (s ip -- ip' f )
   ta1+ true
;

\ Given an ip, scan backwards until you find the cfa.  This assumes
\ that the ip is within a colon definition, and it is not absolutely
\ guaranteed to work, but in practice it nearly always does.
\ This is dependent on the alignment requirements of the machine.
: find-cfa ( ip -- cfa)
   begin
      #align - dup token@    ( ip' token )
      ['] does-ip? token@ =  ( look for the cfa of a : defn )
   until  ( ip)
;

: code ;
: primitive ;

: definer ( cfa-of-child -- cfa-of-defining-word )
  @       ( code-field )
  dup maxprimitive <  if  drop ['] primitive  exit  then
  dup maxcf <=
  if
    case 
      401 of ['] :          endof
      402 of ['] constant   endof
      403 of ['] variable   endof
      404 of ['] user       endof
      405 of ['] defer      endof
      406 of ['] code       endof
      407 of ['] vocabulary endof
    endcase
  else
      find-cfa
  then
;

\ We define these so that the decompiler can find them, but they aren't used
: (;code) ;
: (;uses) ;
: (does>) ;
: >code  ;
\ The decompiler can't quite handle the case statement in C Forth, because
\ it is defined in high level and the branching structure is too hard
\ to distinguish from IF ... THEN (In assembly language versions of Forth,
\ the case statement compiles special run time words (OF) (ENDOF) (ENDCASE) )
\ C Forth will decompile a case statement as a nested IF ... ELSE ... THEN
\ structure, which is logically correct but not as easy to read.

\ : (of) ;		\ Defined in case.m

: (endof) ;
: (endcase) ;
