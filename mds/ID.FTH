\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ For SCCS id's.  Use:
\ id filename.f 1.1 88/01/07
\
\ Then   filename.f ".   will give you the id info.

main definitions
: id  \ rest of line  ( -- string )
   create
   delimiter @ newline =  if   0 ,  else  newline word count ",  then
;

