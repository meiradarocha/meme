\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

create trace-tools.f
only main also hidden definitions
: reasonable-ip? ( ip -- flag)
   dup  origin here between  ( ip flag)
   if   dup aligned =     \ acf's are always aligned
   else drop false
   then
;
: probably-cfa? ( cfa -- flag)
   reasonable-ip?
;
: .current-word ( ip -- )  find-cfa .name  ;
: .last-executed ( ip -- )
   /token - token@  ( cfa)
  dup probably-cfa? 
  if  .name  else  drop ." ??"  then
;
: .caller ( ip -- )
   td 18 to-column ." Called from "
   dup .current-word
   td 56 to-column ." at "
   .
;
only main hidden also main definitions
: .calls ( cfa -- )
  ['] main   ( cfa origin)  \ There is some irrelevant stuff before main
  begin ( cfa search-start )
         2dup here  tsearch  ( cfa last [ found-at ] f )
  while  dup  .caller cr    ( cfa last found-at)
         nip ta1+
  repeat 2drop
;
only main also definitions

