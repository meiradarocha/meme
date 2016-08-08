\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Temporary hex, and temporary decimal.  "h#" interprets the next word
\ as though the base were hex, regardless of what the base happens to be.
\ "d#" interprets the next word as though the base were decimal.
\ "o#" interprets the next word as though the base were octal.
\ "b#" interprets the next word as though the base were binary.

decimal
: #:  \ name  ( base -- )  \ Define a temporary-numeric-mode word
   create , immediate
   does>  base @ >r  @ base !  bl word "compile  r> base !
;

16 #: h#	\ Hex number
10 #: d#	\ Decimal number
 8 #: o#	\ Octal number
 2 #: b#	\ Binary number

\ The old names; use h# and d# instead
16 #: th
10 #: td

