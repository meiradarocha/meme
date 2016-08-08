\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Loads the file if the filename is not defined in the dictionary
: requires  \ filename  ( -- )
   bl word find  if  drop  else "load  then
;

\ Loads the file if wordname is not already defined
: needs  \ wordname filename  ( -- )
   bl word find nip
   if  bl word drop   else  bl word "load  then
; immediate

\ Interprets the line if wordname in not already defined
: \needs  \ wordname rest-of-line  ( -- )
   bl word find nip   if   [compile] \   then
; immediate

