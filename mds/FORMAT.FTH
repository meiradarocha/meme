\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Output Formatting
decimal
: >type   (s adr len -- )
   tuck pad swap cmove   pad swap type  ;
variable lmargin    0 lmargin !
variable rmargin   79 rmargin !
: ?line  (s n -- ) #out @ +    rmargin @ >
  if  cr  lmargin @ spaces  then
;
: ?cr    (s -- )   0 ?line  ;
\ : ??cr   (s -- )   #out @ if cr then ;
: to-column (s column -- )
   #out @  -  1 max spaces
;

variable tabstops  8 tabstops !
: ?to-column ( string-length starting-column -- )
  tuck + rmargin @ >
  if   drop cr  lmargin @ spaces
  else #out @ - spaces
  then
;
: .tab ( string-length -- )
  rmargin @ tabstops @ +  lmargin @
  do
      i  #out @   >=  if  i leave  then
      tabstops @
  +loop   ( string-length target-column )
  ?to-column
;

