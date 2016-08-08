\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Debugger.  Thanks, Mike Perry, Henry Laxen, Mark Smeder.
\
\ The debugger lets you single step the execution of a high level
\ definition.  To invoke the debugger, type debug xxx where xxx is
\ the name of the word you wish to trace.  When xxx executes, you will
\ get a single step trace showing you the word within xxx that
\ is about to execute, and the contents of the parameter stack.
\ Debugging makes everything run slightly slower, even outside
\ the word being debugged.  see debug-off
\
\ debug name	Mark that word for debugging
\ step		Debug in single step mode
\ trace		Debug in trace mode
\ debug-off	Turn off the debugger (makes the system run fast again)
\ resume	Exit from a pushed interpreter (see the f keystroke)
\
\ Keystroke commands while you're single-stepping:
\   d		go down a level
\   u		go up a level
\   c		continue; trace without single stepping
\   g		go; turn off stepping and continue execution
\   m		push a Meme interpreter;  execute "resume" to get back
\   q		abort back to the top level

only main also definitions

vocabulary bug   bug also definitions

\ The following stuff is in the kernel:

\ nuser debugging?  \ On/Off flag
\ nuser 'debug	    \ code field for high level trace
\ nuser <ip         \ lower limit of ip
\ nuser ip>         \ upper limit of ip
\ nuser cnt         \ how many times thru debug next

: upc  ( char -- char' )
   dup ascii a ascii z between  if  ascii a -  ascii A +  then
;

\ Fix the next routine to use the debug version
: pnext   (s -- )  debugging? on  ;

\ Turn off debugging
: unbug   (s -- )  debugging? off  ;

\ hex

only main hidden also main also definitions
bug also definitions
: l.id   (s anf len -- )
   swap dup .id   ( len anf acf )
   c@ th 1f and   ( len namelen )
   - dup 0>= 
	if spaces else drop then
;
variable step? step? on
variable res
: (debug)       (s low-adr hi-adr -- )
   \ Silently refuse to debug the kernel; it's too dangerous
   over  fence @  u<  if  2drop exit  then

   unbug   1 cnt !   ip> !   <ip !   pnext
;
: 'unnest   (s pfa -- pfa' )
   begin   #align + dup token@ ['] unnest = until
;

\ Enter and leave the debugger
variable #out-save variable #line-save
: (debug  ( acf -- )
   /token -   dup 'unnest  (debug)
;
: up1  ( ip -- )  dup find-cfa swap 'unnest (debug)  ;
: (trace   (s - )
   step? @  0=  if
      cr
   then
   ." ( " .s ." )" cr         \ Show stack
   r@ token@ >name td 10 l.id      \ Show word name
   step? @  key? or
   if step? on  res off
      ." [<space> Down Up Continue Meme Go Quit] : "
      key upc
      case
        ascii D  of  					\ Down
	r@ token@ ['] execute <> 
		if
		r@ token@
		else
		dup
		then
	(debug
	endof

        ascii U  of  rp@ na1+ @ up1                     endof \ Up
        ascii C  of  step? @ not step? !                endof \ Continue
        ascii M  of  begin interpret-line res @ until   endof \ Meme
        ascii G  of  <ip off  ip> off                   endof \ Go
        ascii Q  of  cr ." unbug" abort                 endof \ Quit
      endcase
   then
   pnext
;
' (trace  'debug token!

only main bug also main definitions
: (debug
    (debug
;

: debug  \ name (s -- )
   ' (debug
;
: resume (s -- )  res on  pnext  ;
: step   (s -- )  step? on  ;
: trace  (s -- )  step? off ;
: debug-off (s -- )
   unbug
;

only main also definitions
