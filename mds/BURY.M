\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Internal, External, and Bury
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

: Internal	\ -- addr1
	here
;

: External	\ addr1 -- link addr2
	@ here
;

: Bury		\ link addr2 --
	!
;
