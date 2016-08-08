\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

id sift.f  1.1 88/01/07
also hidden
needs sindex substring.f
only main also hidden also definitions
decimal
variable sift-string
: vsift ( voc-cfa -- )
   follow
   begin
      another?
   while 
      dup name>str  ( dictionary-str)
      sift-string @ count rot count sindex ( nfa f)
      -1 <>  if  ?cr .id  else  drop  then
   repeat
;
main definitions
: sift ( str -- )
   sift-string !
   voc-link
   begin  link@ dup  origin u>  while  dup voc> vsift  repeat
   drop
;
: sifting \ name ( -- )
   bl word sift
;
only main also definitions


