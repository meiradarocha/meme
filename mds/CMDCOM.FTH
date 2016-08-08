\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Command parsing tools

decimal
variable cmdbuf 128 allot

variable exit-status

: $+  ( dst-pstr addition-pstr -- dst-pstr )  over "cat  ;
: $0  ( dst-pstr -- dst-pstr )  nullstring over "copy  ;

: rest-of-line  \ whatever  ( -- pstr )
   delimiter @  newline =  if  nullstring  else  newline word  then
;
: sh  \ command-line  ( -- )
   rest-of-line  dup c@  0=  if  drop p" sh"  then  "command
;
: command:  \ name   ( command-head-pstr -- )
   create ",
   does>  ( command-head-pstr )
   cmdbuf $0  swap $+   p"  " $+   rest-of-line $+   "command  ( status )
   exit-status !
;

