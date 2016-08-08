\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ String-array
\ Creates an array of strings.
\ Used in the form:
\ string-array name
\   ," This is the first string in the table"
\   ," this is the second one"
\   ," and this is the third"
\ end-string-array
\
\ name is later executed as:
\
\ name ( index -- addr )
\   index is a number between 0 and one less than the number of strings in
\   the array.  addr is the address of the corresponding packed string.
\   if index is less than 0 or greater than or equal to the number of
\   strings in the array, name aborts with the message:
\        String array index out of range

: string-array ( -- )  ( input stream: name )
  create
  0 ,    ( the number of strings )
  0 ,    ( the starting address of the pointer table )
  does>  ( index pfa )
  2dup @ ( index pfa  index #strings )
  0 swap within  0= abort" String array index out of range"    ( index pfa )
  na1+ token@   ( index table-address )
  swap na+ token@
;
: end-string-array ( -- )
  here align          ( string-end-addr )
  lastacf >body       ( string-end-addr pfa )
  na1+ here over token!    \ Store table address in the second word of the pf
  na1+                ( string-end-addr first-string-addr )
  begin               ( string-end-addr this-string-addr )
      2dup >          ( string-end-addr this-string-addr )
  while
      \ Store string address in table
      dup token,      ( string-end-addr this-string-addr )
      \ Find next string address
      +str
  repeat              ( string-end-addr next-string-addr )
  2drop               ( )
  \ Calculate and store number of strings
  lastacf >body       ( pfa )
  dup na1+ token@     ( pfa table-addr )
  here swap - /n /    ( pfa #strings )
  swap !
;

