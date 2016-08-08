\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

decimal

/n constant /token
/n constant #align

\ FIX: this code should be saving fsp as well
\ : catch  ( acf -- error# | 0 )
\                         ( cfa )  \ Return address is already on the stack
\    sp@ >r               ( cfa )  \ Save data stack pointer
\   handler @ >r         ( cfa )  \ Previous handler
\   rp@ handler !        ( cfa )  \ Set current handler to this one
\   execute              ( )      \ Execute the word passed in on the stack
\    0                    ( 0 )    \ Signify normal completion
\    r> handler !         ( 0 )    \ Restore previous handler
\    r> drop              ( 0 )    \ Restore previous stack pointer
\ ;

\ New version of catch
: catch		( acf -- error# | 0 )
						( cfa )		\ Return address is already on the stack
	sp@ >r				( cfa )		\ Save data stack pointer
	fdup fpop			( f1 f2 cfa ) \ Push the top of float stack onto int stack
	rot rot				( cfa f1 f2 ) \ fpop doesn't change tos!
	>r >r				( cfa )		\ Save it
	fp@ >r				( cfa )		\ Save the float stack pointer
	frp@ >r				( cfa )		\ Save the local var frame pointer
	handler @ >r		( cfa )		\ Previous handler
	rp@ handler !		( cfa )		\ Set current handler to this one
	execute				( )			\ Execute the word passed in on the stack
	0					( 0 )		\ Signify normal completion
	r> handler !		( 0 )		\ Restore previous handler
	r> drop				( 0 )		\ Drop frame pointer
	r> drop				( 0 )		\ Drop float stack pointer
	r> drop r> drop		( 0 )		\ Drop the top of float stack
	r> drop				( 0 )		\ Drop the int stack pointer
;
		
main\ : again  ( sys -- )  compile branch <resolve  ;  immediate

: 1-  ( n1 -- n2 )  1 -  ;
: 2*  ( n1 -- n2 )  2 *  ;
: 2dup  ( n1 n2 -- n1 n2 n1 n2 )  over over  ;
: 2drop  ( n1 n2 -- )  drop drop  ;
: 2swap  ( n1 n2 n3 n4 -- n3 n4 n1 n2 )  rot >r rot r>  ;
: 2over  ( n1 n2 n3 n4 -- n1 n2 n3 n4 n1 n2 )  3 pick  3 pick  ;
: 2@  ( adr -- n1 n2 )  dup /n + @ swap @  ;
: 2!  ( n1 n2 adr -- )  swap over ! /n + !  ;
: -rot  ( n1 n2 n3 -- n3 n1 n2 )  rot rot  ;
: >=  ( n1 n2 -- flag )  < 0=  ;
: u<= ( u1 u2 -- flag )  2dup u< -rot = or  ;
: u>  ( u1 u2 -- flag )  u<= 0=  ;
: u>= ( u1 u2 -- flag )  u<  0=  ;
: <=  ( n1 n2 -- f )  > 0=  ;
: <>  ( n1 n2 -- flag )  = 0=  ;
: 0<> ( n -- flag )  0= 0=  ;
: 0<= ( n -- flag )  dup 0=  swap 0<  or  ;
: 0>= ( n -- flag )  0< 0=  ;
: (s  ( -- )  [compile] (  ; immediate
: pad  ( -- adr )  here 100 +  ;
: space  ( -- )  bl emit  ;
: spaces  ( n -- )  0 max 0 ?do space loop ;
: c,  ( char -- )   here  1 allot  c!  ;
: tuck  ( n1 n2 -- n1 n2 n1 )  swap over  ;
: nip  ( n1 n2 -- n2 )  swap drop  ;
: */  ( n1 n2 n3 -- n4 )  */mod nip  ;

: noop  ( -- )  ;

: aligned  ( adr -- aligned-adr )
   [ #align 1- ] literal +  [ #align negate ] literal and
;
: align  ( -- )  here here aligned swap - allot  ;

: erase  ( adr count -- )  0 fill  ;
: off  ( adr -- )  false swap !  ;
: on  ( adr -- )  true swap !  ;
: umax  ( u1 u2 -- umax )  2dup u<  if  swap  then  drop  ;
: between  ( n min max -- f )  >r over <= swap r> <= and  ;
: within  ( n1 min max+1 -- f )  1- between  ;
: primitive?  ( acf | prim -- flag )  1 maxprimitive between  ;

: octal   ( -- )  8 base !  ;
: binary   ( -- )  2 base !  ;

: blword  \ name  ( -- pstr )
   bl word
;
: load-file  ( pstr -- )  "load  ;

: na1+  ( adr -- adr' )  /n +  ;
: na+  ( adr index -- adr' )  /n * +  ;
: ta1+  ( adr -- adr' )  /token +  ;
: ta+  ( adr index -- adr' )  /token * +  ;

\ Words to follow dictionary links
/n constant /link

: >name  ( acf -- anf )
   begin  1- dup  c@  until      \ Skip over the pad characters
   begin  1- dup  c@  bl < until \ Look for length byte
;
: name>  ( anf -- acf)  count + aligned  ;
: body>  ( apf -- acf )  /n -  ;
: name>str  ( anf -- str )  ;
: >flags  ( acf -- aflags )  >name 1-  ;
: immediate?  ( acf -- f )  >flags c@  ;
: n>link  ( anf -- lfa )  1- /link -  ;  ( skip flag byte and link byte )
: l>name  ( lfa -- acf)  /link +  1+  ;
: >link  ( acf -- lfa )  >name n>link  ;
: lastacf  ( -- acf )  last @ l>name name>  ;
hex
: wbsplit  ( w -- b.low b.high )  dup ff and  swap -8 shift  ff and  ;
: bwjoin  ( b.low b.high -- w )  8 shift or  ;
: lbsplit  ( l -- b.low b.mlow b.mhigh b.high )
   dup ff and
   swap -8 shift  dup ff and
   swap -8 shift  dup ff and
   swap -8 shift
;
: lwsplit  ( l -- w.low w.high )  dup 0ffff and swap -10 shift  ;
: wljoin  ( w.low w.high -- l )  10 shift or  ;
decimal
: <<  ( n count -- n' )  shift  ;
: >>  ( n count -- n' )  negate shift  ;
: >user#  ( acf -- user# )  >body @  ;
: >user  ( apf -- user-adr )  @ up@ +  ;
: 'user#  \ name  ( -- user# )
   '  ( cfa-of-user-variable )  >user#
;
main\ : local ( task-base user-var-addr -- user-var-addr-in-tasks-space )
main\    up@ - +
main\ ;
: .id  ( anf -- )  name>str count type space  ;
decimal
: word-type  ( acf -- word-type )
   @ dup primitive?  if  drop -1 ( code word )  then
;
: ualloc  ( size -- user-number )  #user @  swap #user +!  ;
: nuser  \ name  ( -- )
   /n ualloc user
;

: .name  ( acf -- )  >name .id  ;
: token@  ( adr -- acf)
   @  ( acf| prim )  dup primitive?
       if		\ -- prim
       origin swap	\ -- origin prim
       na+ @		\ -- array.element
       then
;
: crash  ( -- )  \ uninitialized execution vector routine
  r@ /token - token@         ( use the return stack to see who called us )
   dup ['] execute =
   if   'word count type space   else   .name   then
   ." <--deferred word not initialized " abort
;

\    \ We want token!, but using it now would cause a circular definition
\    >user ['] crash  swap (set-relocation-bit)  !
\ ;
: defer ( -- )
     create
     405 here body> !
     ['] crash here set-relocation-bit ! /n allot
;
defer defxx

\ If the acf contains a primitive's token, store the token at adr,
\ else store acf at adr
: token!  ( acf adr -- )
	swap 			\ -- adr acf
	dup				\ -- adr acf acf
	@				\ -- adr acf adr|prim
	primitive?   		\ adr acf false  |  adr adrprim true 
	if
		@ swap !			\ Store the prim at the adr
	else
		over !			\ Store acf at adr ( -- adr )
	set-relocation-bit drop  \ --
	then
;
: token,  ( acf -- )  here  /token allot  token!  ;

\ Like token! but unconditionally sets the relocation bit.  Much preferable
\ to using token! indiscriminately in an environment like NT, where memory that isn't
\ really there (like at address 0) produces a protection fault when it's read.
: reloc! ( n addr -- )
 	set-relocation-bit !
;
: reloc, ( n -- )
	here /n allot reloc!
;

: n! !  ;
\ : !  ( n adr -- )
\    over  origin here  between  if
\       ." ! should be token! "
\       state @  if  ." compiling "  else  ." after "  then
\       lastacf .name cr
\    then
\    !
\ ;

\ Why did these ever call token! or token@ or token, ?
\ Was it to make sure that set-relocation-bit got called?
\ : link@  ( adr -- link )  token@  ;
\ : link!  ( link adr -- )  token!  ;
\ : link,  ( link -- )  token,  ;
: link@		( adr -- link ) @ ;
: link!		( link adr -- ) set-relocation-bit ! ;
: link,		( link -- ) here /link allot link! ;

: deferadr  ( acf type -- data-adr )  drop >body ;
\ (is) is not allowed to store a token in the deferred word, because
\ defer sets the relocation bit on its data field.
: (is)  ( n acf -- data-adr )
\ I added this because the relocation bit is already set on the deferred
\ CFA field.
   over maxprimitive u< if ." (is): Can't store token in deferred word. Token = "
			swap drop . abort
		      then
   dup word-type               ( n acf code-field-word )
\ Defer has set the relocation bit, so this code really means nothing
   dup ['] #user word-type  =  if  deferadr !      exit  then
   dup ['] defxx word-type  =  if  deferadr set-relocation-bit ! exit  then
main\ dup ['] main word-type  =  if  deferadr set-relocation-bit ! exit  then
mod\  dup [ ' main word-type ] literal = if  deferadr set-relocation-bit ! exit  then
   drop >body !
;

: is  \ name  ( val -- )
   state @  if   [compile] ['] compile (is)  else  ' (is)  then
; immediate

: .x  base @ swap hex . base !  ;

\ The following definitions are implementation-independent

decimal

\ Why token@ and token!?  This looks way wrong to me. -MdG
: definitions  ( -- )  context token@ current token!  ;
\ : definitions  ( -- )  context @ current reloc!  ;

\ Determine if the user wants to abort a listing or something.
defer exit?  ( -- flag )
' key? is exit?

: bounds  ( adr len -- endadr startadr )  over + swap  ;

\ A convenient word for stepping through and displaying a range of locations
: ..  ( adr -- adr' )  dup @ u. na1+  ;

\ A much better dump utility is loaded in a later file.  However, in the
\ porting stage, it is often nice to have a very simple dump which may
\ work even if some things are broken
: ndump  ( adr count -- )  bounds  ?do  i @ .  /n +loop  ;

: do-defined  ( acf [ -1 | 0 | 1 ] -- ?? )
   state @  if
      0>  if  execute  else  token,  then
   else
      drop execute
   then
;

: ?literal  ( n -- )  state @  if  [compile] literal  then  ;

\ True if n is a printable character.  Needs to be modified for EBCDIC!
: printable?  ( n -- flag )  bl 127 within  ;
: ascii  \ name  ( -- n )
   bl word 1+ c@  ?literal
; immediate 
: control  \ name  ( -- n )
   bl word 1+ c@  bl 1- and  ?literal
; immediate

create "temp 256 allot

: "copy  ( from to -- )  over c@ 2+ cmove  ;
: move  ( from to len -- )
   -rot  2dup u< if  rot cmove>   else  rot  cmove then
;
: place  ( adr len to-adr -- )  2dup c!  2dup + 1+  0 swap c!  1+ swap move  ;
: pack  ( adr len to-adr -- to-adr )  dup >r place r>  ;

\ Changed by MdG
\ : ",  ( adr len -- )  tuck here place  ( len )  1+ allot align  ;
: ",  ( adr len -- )  tuck here place  ( len )  2+ allot align  ;
: ,"  \ string"  ( -- )
   ascii " word count ",
;

260 buffer: string2
: save-string  ( pstr1 -- pstr2 )  count string2 pack  ;

create nullstring 0 ,

\ Append startstr to the end of endstr
: "cat  ( startstr endstr -- )
   2dup c@ swap c@ + >r
   swap count 1+ 2 pick dup c@ + 1+ swap cmove r> swap c!
;

\ Changed by MdG
: skipstr  ( -- adr len )
   r> r> count     ( return-adr adr len )
\   2dup + aligned  ( return-adr adr len new-ip )
   2dup + 1+ aligned  ( return-adr adr len new-ip )
   >r rot >r
;

\ MdG - Now primitives
\ : ("s)  ( -- str-adr )  skipstr drop 1-  ;
\ : (")  ( -- adr len )  skipstr  ;
\ : (c") ( -- cstr-adr ) skipstr drop ;

: ["]  \ name  ( -- pstr )  \ For backwards compatibility; obsolete (use p" )
   compile ("s)    ,"
;   immediate
: [""]  \ name  ( -- pstr ) \ For backwards compatibility; obsolete (use "" )
   compile ("s)  bl word count ",
; immediate
: ""  \ name  ( -- pstr )
   state @  if
      compile ("s)  bl word count  ",
   else
      bl word  "temp "copy "temp
   then
; immediate
: p"  \ string"  ( -- pstr )
   state @  if
      compile ("s)  ,"
   else
      ascii " word  "temp "copy "temp
   then
; immediate
: "  \ string  ( -- adr len )
   state @  if
      compile (")  ,"
   else
      ascii " word  "temp "copy "temp count
   then
; immediate
: c"  \ string  ( -- cstr-adr )
   state @  if
      compile (c")  ,"
   else
      ascii " word  "temp "copy "temp count drop
   then
; immediate
: ".  ( str -- )  count type  ;

defer error-output  ( -- )
' noop is error-output  \ XXX Should select standard error

defer restore-output  ( -- )
' noop is restore-output  \ XXX Should reselect standard output

: (where  ( -- )
   input-file-name c@  if
      ."  File: " input-file-name count type space
      state @  if  ." Compiling "  else  ." Latest word was "  then
      lastacf .name  cr
   then
;

defer where
' (where is where

: ?stack  ( -- )
   sp@  sp0 @  swap   u<  if
      error-output ." Stack Underflow " where restore-output
      sp0 @ sp!  abort
   then
   sp@  sp0 @ 400 -  u<  abort" Stack Overflow "
;
: (.s  ( -- )  depth 0 ?do  depth i - 1- pick .  loop  ;
: .s  ( -- )  ?stack  depth  if  (.s  else  ." Empty "  then  ;

nuser csp
: !csp  ( -- )  sp@ csp !  ;
: ?csp  ( -- )
   sp@ csp @ <>
   if error-output ." Stack Changed " where restore-output abort then
;

: ?  ( adr -- )  @ .  ;

\ Alias makes a new word which behaves exactly like an existing
\ word.  This works whether the new word is encountered during
\ compilation or interpretation, and does the right thing even
\ if the old word is immediate.

: alias  \ new-name old-name  ( -- )
   create hide
   bl word canonical find    ( acf -1 | acf 1  |  str false )
   dup if
      swap token, , immediate
   else
      drop  ." can't find "  count type
   then  reveal
   does>  dup token@ swap ta1+ @  ( acf -1 | acf 1 )
   do-defined
;

: s>d  ( n -- d )  dup 0<  ;
: u>d  ( u -- d )  0  ;

1 constant /c
2 constant /w
4 constant /l

alias ca1+ 1+  ( adr -- adr' )
alias wa1+ 2+  ( adr -- adr' )
: la1+  ( adr1 -- adr2 )  /l +  ;

: /c*  ( n1 -- n2 )  ;
: /w*  ( n1 -- n2 )  dup +  ;
: /l*  ( n1 -- n2 )  2 <<  ;

alias ca+ +  ( adr1 n -- adr2 )
: wa+  ( adr1 n -- adr2 )  /w* +  ;
: la+  ( adr1 n -- adr2 )  /l* +  ;

: w,  ( w -- )  here /w allot w!  ;
hex
: <w@  ( adr -- signed.w )  w@ dup 8000 and if 7fff and negate then ;


decimal
main\ 16 constant #vocs	\ Must agree with NVOCS in forth.h
1 constant #threads	\ Must agree with NTHREADS in forth.h

main\ : voc-link,  ( -- )  \ links this vocabulary to the chain
main\   here   voc-link link@  link,   voc-link link!
main\ ;
main\ : vocabulary  \ name  ( -- )
main\    create
main\    #threads 0  do  origin link,  loop
main\    voc-link,          \ voc-link
main\    does>  body> context token!
main\ ;

\ The also/only vocabulary search order scheme
main\ : v>threads  ( voc-link-adr -- threads )  #threads /link * -  ;
main\ : voc>  ( voc-link-adr -- acf )  v>threads  body>  ;
main\ : >threads  ( acf -- threads-adr )  ;   \ For compatibility of user code
main\ : >xthreads  ( acf -- voc-ptr-adr )  >body  ;

main\ context dup token@ swap ta1+ token!   \ make forth also
main\ vocabulary root  root definitions
main\ : also  ( -- )
main\    context dup ta1+ #vocs 2- /token * cmove>
main\ ;
main\ : only  ( -- )
main\    #vocs 0  do
main\       origin  context i ta+  token!
main\    loop
main\    ['] root  context #vocs 1- ta+  token!
main\    root
main\ ;
\ XXX This implementation is not in accordance with the standard.
\ It should not require an argument.
main\ : seal  \ vocabulary-name  ( -- )
main\    ' >body   context #vocs /n * erase   context token!
main\ ;
main\ : previous  ( -- )
main\    context dup ta1+ swap #vocs 2- /token * cmove
main\    context #vocs 2- /token * +  origin swap token!
main\ ;

main\ : main  ( -- )  main  ;
mod\  : main [ ' main origin - ] literal origin + execute ;
main\ : definitions  ( -- )  definitions   ;
main\ : order  ( -- )
main\	." context: " context
main\	#vocs 0
main\	do
main\		dup token@ 
main\		dup origin <>
main\		if  
main\			.name  
main\		else
main\			drop
main\		then  
main\		ta1+  
main\	loop 
main\	drop
main\	4 spaces ." current: " current token@  .name
main\ ;
main\ : vocs  ( -- )
main\    voc-link link@
main\    begin   dup voc> .name  link@ dup origin u<=  until
main\    drop
main\ ;
main\ variable largest
main\ : follow  ( voc -- )  >xthreads largest link!  ;
main\ 
main\ \ Modified to work with code modules -MdG
main\ : another?  ( -- [ anf ] end? )
main\ \   largest link@  link@ dup  origin u>  if  ( alf )
main\    largest link@  link@ dup  @ 0<>  if  ( alf )
main\       dup largest link!
main\       na1+ ca1+      \ skip link field and flag byte
main\       true               ( anf true )
main\    else
main\       drop false         ( false )
main\    then
main\ ;

main\ only main also definitions
main\ vocabulary hidden
main\ : ?missing  ( f -- )
main\    if   error-output 'word count type ."  ?" restore-output abort  then
main\ ;
main\ : dp!  ( adr -- )  here - allot  ;
main\ nuser fence
main\ : trim  ( fadr voc-adr -- )
main\    #threads 0  do
main\       2dup  begin  link@  2dup u>  until  ( fadr thread  fadr link' )
main\       nip over link!              ( fadr thread )
main\       /link +
main\    loop
main\    2drop
main\ ;
main\ \ It is a bad idea to do a forget that will result in the forgetting of
main\ \ vocabularies that are presently in the search order.
main\ : ((forget   ( acf -- )
main\ 	>link				\ ( acf -- adr )
main\ 	dup fence link@ u< abort" below fence"  ( -- adr )
main\ 
main\ 	\ first forget any vocabularies defined since the meme to forget.
main\ 	\ Loop, searching the vocab chain for the first one that's lower
main\ 	\ in memory than the forgotten meme.
main\ 	dup voc-link			\ ( -- adr adr voc-link-adr )
main\ 		begin			\ For each vocabulary in linked list
main\ 		link@ 2dup  u>=		\ Get next link, loop till adr greater
main\ 		until			\ ( -- adr adr voc-link-adr )
main\ 	dup voc-link link!		\ ( -- adr adr voc-link-adr )
main\ 	nip				\ ( -- adr voc-link-adr )
main\ 
main\    \ now, for all remaining vocabularies, forget words defined
main\    \ since the word to forget
main\    begin
main\       dup origin u>=  ( any more vocabularies? )
main\    while
main\       2dup v>threads  ( adr voc-link-adr adr voc-threads-adr )
main\       trim            ( adr voc-link-adr )
main\       link@           ( adr new-voc-link-adr )
main\    repeat
main\    drop   dp!
main\ ;
main\ 
main\ defer (forget
main\ ' ((forget is (forget
main\ 
main\ : forget   ( -- )
main\    bl word canonical  current @ vfind 0= ?missing  (forget
main\ ;
main\ 
main\ only main also definitions
: ?comp  ( -- )  state @  0= abort" Compilation Only "  ;
: ?exec  ( -- )  state @     abort" Execution Only "  ;
: ?pairs  ( -- )  - abort" Conditionals not paired "  ;

main\ nuser hld
: hold  ( char -- )  -1 hld +!   hld @ c!  ;
: <#    ( -- )  pad  hld  !  ;
: #>  ( d# -- adr len )  2drop  hld  @  pad  over  -  ;
: sign  ( d# n1 -- d# )  0< if  ascii -  hold  then  ;
: mu/mod  (s d# n1 -- rem d#quot )
   >r  0  r@  um/mod  r>  swap  >r  um/mod  r>
;
: #  ( n1 d# -- n1 d# )
   base @ mu/mod           ( n1 nrem d# )
   rot                     ( n1 d# nrem )
   dup 9 >  if  10 - ascii a +  else  ascii 0 +  then  ( n1 d# nrem' )
   hold
;
: d0=  ( d -- flag )  or 0=  ;
: #s  ( n1 d# -- n1 d#' )  begin  #  2dup d0=  until  ;

: (u.)  ( u -- a len )  u>d  <# #s #>  ;
warning off
: u.  ( u -- )  (u.)  type space  ;
warning on
: u.r  ( u len -- )  >r   (u.)   r> over - spaces  type  ;
: (.)  ( n -- a len )  dup abs s>d   <# #s  rot sign   #>  ;
: s.  ( n -- )  (.)   type space  ;
: .r  ( n l -- )  >r  (.)  r> over - spaces  type  ;
16\ : l.  ( l -- )  tuck labs  <# #s nlswap sign #> type space  ;
16\ : ul.  ( l -- )  <# #s #> type space  ;

32\ : /n*  ( n1 -- n2 )  2 <<  ;
16\ : /n*  ( n1 -- n2 )  dup +  ;
: clear  ( -- )  begin  depth  while  drop  repeat  ;
: ??cr  ( -- )  #out @  if  cr  then  ;

\ Modes for opening files
0 constant read
1 constant write
2 constant modify

: literal?  ( str -- str false  | literal true )
   >r r@ number?  if   ( l )
      r> drop  true
   else
      drop r> false
   then
;
