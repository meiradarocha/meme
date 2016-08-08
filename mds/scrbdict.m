\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Scribble the dictionary
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

\
\
\ == TO USE ==
\
\ fload this file, then type p" memeview.dic" save-scribbled-dictionary
\
\

\
\
\ *****************************************************************************
\ These memes hide ExecuteStartupModule from the ravages of ScribbleDict,
\ and then restore it as the only meme in the main vocabulary.

variable UNLlfa					\ LFA of UnloadModule
' UnloadModule >link UNLlfa reloc!

variable ESMlfa					\ LFA of ExecuteStartupModule
' ExecuteStartupModule >link ESMlfa reloc!

variable NClfa					\ LFA of NewCompound
' NewCompound >link NClfa reloc!

variable MAINlfa				\ LFA of main
' main >link MAINlfa reloc!

variable mainthread1			\ Addr of 1st thread in main
' main /n + mainthread1 reloc!

\ *****************************************************************************
\ A meme to destroy the name fields and link fields in the dictionary.
\
\ rdaddr points at the next cell from which to read the bytes
\ that are written over the name and link fields

variable rdaddr

: rdaddr! ( -- )				\ Initialize rdaddr
	here /n - rdaddr !
;

: rdaddr++ ( -- ) 
	/n negate rdaddr +!			\ Bump to next cell address
; 

: #scribble ( lfa -- n ) 
	/n +						\ -- nfa
	1+ c@						\ -- lengthbyte
	2 + aligned /n /			\ -- number.of.cells.in.nf
	1+ 							\ -- number.of.cells.in.nf.and.lf
;

: scribble1 ( addr -- ) 		\ Scribble one cell
	rdaddr @ @ swap ! 			\ Write what rdaddr points at into addr
	rdaddr++ 					\ Bump ptr
;

: scribble  ( addr n -- ) 		\ Scribble n cells
	/n * over + swap
	?do 
		i scribble1 
	/n +loop 
;

\ Scribble the thread.  If the meme is ExecuteStartupModule, leave
\ it intact
: scribblethread ( linkfieldptraddr -- )
	dup @						\ -- lfpaddr firstlfa
	swap scribble1				\ -- firstlfa
	begin						\ -- lfa
		dup @					\ -- lfa nextlfa
		dup 					\ -- lfa nextlfa nextlfa
	while						\ -- lfa nextlfa
		swap					\ -- nextlfa lfa
		dup ESMlfa @ <> over NClfa @ <> and over MAINlfa @ <> and
		over UNLlfa @ <> and
		if
			dup #scribble		\ -- nextlfa lfa n
			scribble			\ -- nextlfa
		else
			drop				\ -- nextlfa
		then
	repeat
	2drop						\ --
;

: ScribbleVoc	( voc-link-addr -- )
	v>threads
	#threads 0
	do
		dup scribblethread		\ -- lfpaddr
		origin over reloc!		\ -- lfpaddr	Make thread look empty
		/n +					\ -- nextlfpaddr
	loop
	drop
;

\ The standard library is not in the list of vocabularies,
\ so it must be handled separately.
: scribble-std.ml	( -- )
	std.mlOrigin 				\ -- voc-cfa
	/n +						\ voc-cfa -- threads-addr
	#threads /link * +			\ threads-addr -- voc-link-addr
	ScribbleVoc					\ voc-link-addr --
;
	
: ScribbleDict		( -- )
	rdaddr!						\ --		Init ptr for src of garbage
	scribble-std.ml				\ --
	voc-link link@				\ -- vladdr
	begin
		?dup					\ -- vladdr vladdr
	while						\ -- vladdr
		dup ScribbleVoc			\ -- vladdr
		link@					\ -- nextvladdr
	repeat

	\ Make ExecuteStartupModule the only meme in the main vocab
	UNLlfa @ mainthread1 @ !	\ Point main's 1st thread at the link field
								\ of UnloadModule
	ESMlfa @ UNLlfa @ !			\ Point LFA of UnloadModule at ESM
	NClfa @ ESMlfa @ !			\ Point LFA of ESM at NewCompound
	MAINlfa @ NClfa @ !			\ Point LFA of NewCompound at main
	origin MAINlfa @ !			\ Point the link field at the origin
;

: save-scribbled-dictionary		\ pstring --
	cr ." Scribbling... "
	ScribbleDict
	cr ." Saving... "
	save-dictionary
	cr ." Exiting... "
	bye
;

;s		\ Stop loading the file

\ ****************************************************************************
\ Vocabulary for testing and debugging

.( defining fnord )
only main also definitions
vocabulary fnord
fnord definitions
: foo ; : bar ;
only main also definitions

