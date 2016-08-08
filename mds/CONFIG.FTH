\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Configuration "smart-comments".  Define the name for your machine,
\ and comment out the names of the other machines.  Then you can use
\ \? to include or exclude system-dependent lines of code.  For instance,
\ if  vax is defined and sun2.0 is not defined, then
\    \? vax     .( I am a Vax) cr
\    \? sun2.0  .( I am a Sun) cr
\ will print "I am a Vax"

\ : vax ;
\ : sun2.0  ;
\ : sun3.0  ;

\ Comments out the rest of the line if flag is false.
: ?\  ( flag -- )   0=  if  [compile] \  then  ;  immediate

: \?  \ name  ( -- )
   bl word find  swap drop  [compile] ?\
; immediate
