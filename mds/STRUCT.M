\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Structures for Meme.
\ By Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

\ C-like structs
\
\ -ByteFields is a metadefining meme for datatypes in structs.
\ 8 -ByteFields Floats : Float 1 Floats ;
\ creates a defining meme, "Floats", which is used to define 8-byte
\ fields in a struct.
\ Structures are built as follows:
\ BeginStruct
\ 	Float X
\	Float Y
\	Float Z
\ Struct Points
\ BeginStruct
\	1 Points vertex1
\	1 Points vertex2
\	1 Points vertex3
\ Struct Triangles
\ SizeOf Triangles \ returns 72
\ 1 ElementOf Triangles \ adds 72 to top of stack
\
\ Stack behavior follows, first for -ByteFields, then for the does>
\ portion of -ByteFields, then for the does> portion within the does>
\ portion.
( #bytes -- ) ( [does] len #fields -- newlen ) ( [does] baseaddr -- fieldaddr )
: -ByteFields
	create ,	\ compile the # of bytes in these fields
	does> @		( len #fields #bytes )
		create
		dup ,	\ compile # of bytes in this data type
		rot dup , ( #fields #bytes len ) \ compile the byte offset
		rot rot * +
		does> /n + @ +
;

/n -ByteFields Ints		: Int 1 Ints ;
/n -ByteFields Ptrs		: Ptr 1 Ptrs ;
/w -ByteFields Shorts	: Short 1 Shorts ;
/f -ByteFields Floats	: Float 1 Floats ;
/f -ByteFields IFloats	: IFloat 1 IFloats ;
/c -ByteFields Chars	: Char 1 Chars ;
0  -ByteFields Lbls		: Lbl 1 Lbls ;

: BeginStruct ( -- initlen ) 0 ;
: Struct ( len -- ) -ByteFields ;

: SizeOf ( -- size ) ' /n + @ ;
: [SizeOf] ( -- ) SizeOf [compile] literal ; immediate

: ElementOf ( baseaddr element# -- fieldaddr ) SizeOf * + ;
: [ElementOf] ( -- ) ' /n + @ [compile] literal compile * compile + ; immediate

: OffsetOf 0 ' execute ;
: [OffsetOf] OffsetOf [compile] literal ; immediate
