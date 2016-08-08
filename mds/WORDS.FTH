\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Display the MEMES in the Context Vocabulary

only main also definitions
: memes   (s -- )
	0 lmargin !  td 64 rmargin !  td 14 tabstops !
	??cr
	context token@ follow
	begin   another?
	while                     ( nfa )
		pause
		dup c@ th 1f and  .tab  ( nfa ) .id
		key? if
			key
			case 
				th 1b 	of exit endof 
				dup	of begin pause key? until key
					case 
						th 1b of exit endof
						dup of endof
					endcase
				endof
			endcase
		then
	repeat
;

only definitions main also
: memes    memes ;
only main also definitions
