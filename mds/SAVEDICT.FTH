\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Saves a copy of the current Forth dictionary in a file.
\ The format of this file is system-independent.

\ Save-forth should probably be a primitive, since this code has to agree
\ in detail with the read_dictionary() C procedure.

\ The functions provided by this file are:
\ save-dictionary  ( file-name -- )
\	Creates a file whose name is given by the packed string file-name
\	and saves an image of the currently-executing Forth system
\	in that file.  That file can later be used to initialize the
\	Forth dictionary to contain all the definitions that
\	were in the dictionary when save-dict was executed.

variable fd
hex

create header  8 /n* allot
\ magic  serial#  dict-start dict-size  user-start user-size  entry   res

: >relmap  ( start #bytes -- relmap-start #reloc-bytes )
   bounds                ( end start )
   >relbit               ( end relmap-start start-bitmask )
   rot >relbit           ( relmap-start start-bitmask relmap-end end-bitmask )
   \ We assume that the bitmask for the start of the relocatable space
   \ represents the first bit in a relocation byte.
   rot <>  if  1+  then ( relmap-start relmap-next )
   over -
;
: text-size  ( -- n )  here origin -  ;
: (save-dict) ( cfa str -- )
   swap maintask local turnkey set-relocation-bit !
   dup  write  fopen  fd !   ( str fd )  \ make a new file
   fd @ 0=  if  ." can't create " ".  cr  abort  then
   drop                                  \ get rid of filename

16\ 5820
32\ 581120
		   					header 0 na+  !   \ Magic number
   0						header 1 na+  !   \ Serial Number
   origin					header 2 na+  n!  \ Dictionary start address
   text-size				header 3 na+  !   \ Dictionary size
   maintask					header 4 na+  !   \ User area address
   maintask local #user @ 	header 5 na+  !   \ User area address
   origin					header 6 na+  n!  \ Origin
   0						header 7 na+  !   \ Reserved

   header  8 /n*          fd @  fputs
   origin  text-size      fd @  fputs
   up@     #user @        fd @  fputs

   origin  text-size  >relmap  fd @  fputs
   up@     #user @    >relmap  fd @  fputs

   fd @  fclose
;


: save-dictionary	( pstring -- )		0 swap	(save-dict) ;
: save-turnkey		( cfa pstring -- )			(save-dict) ;
   
