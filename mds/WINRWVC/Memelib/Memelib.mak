# Microsoft Developer Studio Generated NMAKE File, Based on Memelib.dsp
!IF "$(CFG)" == ""
CFG=Memelib - Win32 Release
!MESSAGE No configuration specified. Defaulting to Memelib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Memelib - Win32 Release" && "$(CFG)" !=\
 "Memelib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Memelib.mak" CFG="Memelib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Memelib - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Memelib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Memelib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Memelib.dll"

!ELSE 

ALL : "$(OUTDIR)\Memelib.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Allocdic.obj"
	-@erase "$(INTDIR)\bitcon.obj"
	-@erase "$(INTDIR)\Chario.obj"
	-@erase "$(INTDIR)\Cyberspc.obj"
	-@erase "$(INTDIR)\debuglog.obj"
	-@erase "$(INTDIR)\Decser.obj"
	-@erase "$(INTDIR)\dkmodule.obj"
	-@erase "$(INTDIR)\expiry.obj"
	-@erase "$(INTDIR)\Extend.obj"
	-@erase "$(INTDIR)\Forth.obj"
	-@erase "$(INTDIR)\Globals.obj"
	-@erase "$(INTDIR)\Init.obj"
	-@erase "$(INTDIR)\Io.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\Matherr.obj"
	-@erase "$(INTDIR)\Meme.obj"
	-@erase "$(INTDIR)\MEME.res"
	-@erase "$(INTDIR)\Memejoy.obj"
	-@erase "$(INTDIR)\Mememat.obj"
	-@erase "$(INTDIR)\Mememaus.obj"
	-@erase "$(INTDIR)\memenet.obj"
	-@erase "$(INTDIR)\Memerend.obj"
	-@erase "$(INTDIR)\Memeser.obj"
	-@erase "$(INTDIR)\Mstimer.obj"
	-@erase "$(INTDIR)\Often.obj"
	-@erase "$(INTDIR)\rfcache.obj"
	-@erase "$(INTDIR)\Rfile.obj"
	-@erase "$(INTDIR)\Syscall.obj"
	-@erase "$(INTDIR)\Util.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\Winglobl.obj"
	-@erase "$(OUTDIR)\Memelib.dll"
	-@erase "$(OUTDIR)\Memelib.exp"
	-@erase "$(OUTDIR)\Memelib.lib"
	-@erase "$(OUTDIR)\Memelib.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /ML /W3 /Gi /GX /O2 /I "d:\meme" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "RWFLOAT" /Fp"$(INTDIR)\Memelib.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MEME.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Memelib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib rwl21.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /version:2.7 /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\Memelib.pdb" /map:"$(INTDIR)\Memelib.map"\
 /machine:I386 /out:"$(OUTDIR)\Memelib.dll" /implib:"$(OUTDIR)\Memelib.lib"\
 /libpath:"e:\rwwin21\lib" 
LINK32_OBJS= \
	"$(INTDIR)\Allocdic.obj" \
	"$(INTDIR)\bitcon.obj" \
	"$(INTDIR)\Chario.obj" \
	"$(INTDIR)\Cyberspc.obj" \
	"$(INTDIR)\debuglog.obj" \
	"$(INTDIR)\Decser.obj" \
	"$(INTDIR)\dkmodule.obj" \
	"$(INTDIR)\expiry.obj" \
	"$(INTDIR)\Extend.obj" \
	"$(INTDIR)\Forth.obj" \
	"$(INTDIR)\Globals.obj" \
	"$(INTDIR)\Init.obj" \
	"$(INTDIR)\Io.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\Matherr.obj" \
	"$(INTDIR)\Meme.obj" \
	"$(INTDIR)\MEME.res" \
	"$(INTDIR)\Memejoy.obj" \
	"$(INTDIR)\Mememat.obj" \
	"$(INTDIR)\Mememaus.obj" \
	"$(INTDIR)\memenet.obj" \
	"$(INTDIR)\Memerend.obj" \
	"$(INTDIR)\Memeser.obj" \
	"$(INTDIR)\Mstimer.obj" \
	"$(INTDIR)\Often.obj" \
	"$(INTDIR)\rfcache.obj" \
	"$(INTDIR)\Rfile.obj" \
	"$(INTDIR)\Syscall.obj" \
	"$(INTDIR)\Util.obj" \
	"$(INTDIR)\Winglobl.obj"

"$(OUTDIR)\Memelib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Memelib.dll" "$(OUTDIR)\Memelib.bsc"

!ELSE 

ALL : "$(OUTDIR)\Memelib.dll" "$(OUTDIR)\Memelib.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Allocdic.obj"
	-@erase "$(INTDIR)\Allocdic.sbr"
	-@erase "$(INTDIR)\bitcon.obj"
	-@erase "$(INTDIR)\bitcon.sbr"
	-@erase "$(INTDIR)\Chario.obj"
	-@erase "$(INTDIR)\Chario.sbr"
	-@erase "$(INTDIR)\Cyberspc.obj"
	-@erase "$(INTDIR)\Cyberspc.sbr"
	-@erase "$(INTDIR)\debuglog.obj"
	-@erase "$(INTDIR)\debuglog.sbr"
	-@erase "$(INTDIR)\Decser.obj"
	-@erase "$(INTDIR)\Decser.sbr"
	-@erase "$(INTDIR)\dkmodule.obj"
	-@erase "$(INTDIR)\dkmodule.sbr"
	-@erase "$(INTDIR)\expiry.obj"
	-@erase "$(INTDIR)\expiry.sbr"
	-@erase "$(INTDIR)\Extend.obj"
	-@erase "$(INTDIR)\Extend.sbr"
	-@erase "$(INTDIR)\Forth.obj"
	-@erase "$(INTDIR)\Forth.sbr"
	-@erase "$(INTDIR)\Globals.obj"
	-@erase "$(INTDIR)\Globals.sbr"
	-@erase "$(INTDIR)\Init.obj"
	-@erase "$(INTDIR)\Init.sbr"
	-@erase "$(INTDIR)\Io.obj"
	-@erase "$(INTDIR)\Io.sbr"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\Main.sbr"
	-@erase "$(INTDIR)\Matherr.obj"
	-@erase "$(INTDIR)\Matherr.sbr"
	-@erase "$(INTDIR)\Meme.obj"
	-@erase "$(INTDIR)\MEME.res"
	-@erase "$(INTDIR)\Meme.sbr"
	-@erase "$(INTDIR)\Memejoy.obj"
	-@erase "$(INTDIR)\Memejoy.sbr"
	-@erase "$(INTDIR)\Mememat.obj"
	-@erase "$(INTDIR)\Mememat.sbr"
	-@erase "$(INTDIR)\Mememaus.obj"
	-@erase "$(INTDIR)\Mememaus.sbr"
	-@erase "$(INTDIR)\memenet.obj"
	-@erase "$(INTDIR)\memenet.sbr"
	-@erase "$(INTDIR)\Memerend.obj"
	-@erase "$(INTDIR)\Memerend.sbr"
	-@erase "$(INTDIR)\Memeser.obj"
	-@erase "$(INTDIR)\Memeser.sbr"
	-@erase "$(INTDIR)\Mstimer.obj"
	-@erase "$(INTDIR)\Mstimer.sbr"
	-@erase "$(INTDIR)\Often.obj"
	-@erase "$(INTDIR)\Often.sbr"
	-@erase "$(INTDIR)\rfcache.obj"
	-@erase "$(INTDIR)\rfcache.sbr"
	-@erase "$(INTDIR)\Rfile.obj"
	-@erase "$(INTDIR)\Rfile.sbr"
	-@erase "$(INTDIR)\Syscall.obj"
	-@erase "$(INTDIR)\Syscall.sbr"
	-@erase "$(INTDIR)\Util.obj"
	-@erase "$(INTDIR)\Util.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\Winglobl.obj"
	-@erase "$(INTDIR)\Winglobl.sbr"
	-@erase "$(OUTDIR)\Memelib.bsc"
	-@erase "$(OUTDIR)\Memelib.dll"
	-@erase "$(OUTDIR)\Memelib.exp"
	-@erase "$(OUTDIR)\Memelib.lib"
	-@erase "$(OUTDIR)\Memelib.map"
	-@erase "$(OUTDIR)\Memelib.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /ML /W3 /Gm /Gi /GX /Zi /Od /Gy /I "d:\meme" /D "_DEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "RWFLOAT" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\Memelib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MEME.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Memelib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Allocdic.sbr" \
	"$(INTDIR)\bitcon.sbr" \
	"$(INTDIR)\Chario.sbr" \
	"$(INTDIR)\Cyberspc.sbr" \
	"$(INTDIR)\debuglog.sbr" \
	"$(INTDIR)\Decser.sbr" \
	"$(INTDIR)\dkmodule.sbr" \
	"$(INTDIR)\expiry.sbr" \
	"$(INTDIR)\Extend.sbr" \
	"$(INTDIR)\Forth.sbr" \
	"$(INTDIR)\Globals.sbr" \
	"$(INTDIR)\Init.sbr" \
	"$(INTDIR)\Io.sbr" \
	"$(INTDIR)\Main.sbr" \
	"$(INTDIR)\Matherr.sbr" \
	"$(INTDIR)\Meme.sbr" \
	"$(INTDIR)\Memejoy.sbr" \
	"$(INTDIR)\Mememat.sbr" \
	"$(INTDIR)\Mememaus.sbr" \
	"$(INTDIR)\memenet.sbr" \
	"$(INTDIR)\Memerend.sbr" \
	"$(INTDIR)\Memeser.sbr" \
	"$(INTDIR)\Mstimer.sbr" \
	"$(INTDIR)\Often.sbr" \
	"$(INTDIR)\rfcache.sbr" \
	"$(INTDIR)\Rfile.sbr" \
	"$(INTDIR)\Syscall.sbr" \
	"$(INTDIR)\Util.sbr" \
	"$(INTDIR)\Winglobl.sbr"

"$(OUTDIR)\Memelib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=wsock32.lib rwl21.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /version:2.7 /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\Memelib.pdb" /map:"$(INTDIR)\Memelib.map"\
 /debug /machine:I386 /out:"$(OUTDIR)\Memelib.dll"\
 /implib:"$(OUTDIR)\Memelib.lib" /libpath:"e:\rwwin21\lib" 
LINK32_OBJS= \
	"$(INTDIR)\Allocdic.obj" \
	"$(INTDIR)\bitcon.obj" \
	"$(INTDIR)\Chario.obj" \
	"$(INTDIR)\Cyberspc.obj" \
	"$(INTDIR)\debuglog.obj" \
	"$(INTDIR)\Decser.obj" \
	"$(INTDIR)\dkmodule.obj" \
	"$(INTDIR)\expiry.obj" \
	"$(INTDIR)\Extend.obj" \
	"$(INTDIR)\Forth.obj" \
	"$(INTDIR)\Globals.obj" \
	"$(INTDIR)\Init.obj" \
	"$(INTDIR)\Io.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\Matherr.obj" \
	"$(INTDIR)\Meme.obj" \
	"$(INTDIR)\MEME.res" \
	"$(INTDIR)\Memejoy.obj" \
	"$(INTDIR)\Mememat.obj" \
	"$(INTDIR)\Mememaus.obj" \
	"$(INTDIR)\memenet.obj" \
	"$(INTDIR)\Memerend.obj" \
	"$(INTDIR)\Memeser.obj" \
	"$(INTDIR)\Mstimer.obj" \
	"$(INTDIR)\Often.obj" \
	"$(INTDIR)\rfcache.obj" \
	"$(INTDIR)\Rfile.obj" \
	"$(INTDIR)\Syscall.obj" \
	"$(INTDIR)\Util.obj" \
	"$(INTDIR)\Winglobl.obj"

"$(OUTDIR)\Memelib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "Memelib - Win32 Release" || "$(CFG)" ==\
 "Memelib - Win32 Debug"
SOURCE=..\Allocdic.c
DEP_CPP_ALLOC=\
	"..\config.h"\
	
NODEP_CPP_ALLOC=\
	"..\externs.h"\
	"..\forth.h"\
	"..\protos.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Allocdic.obj" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Allocdic.obj"	"$(INTDIR)\Allocdic.sbr" : $(SOURCE) $(DEP_CPP_ALLOC)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\bitcon.c
DEP_CPP_BITCO=\
	"..\..\bitcon.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\bitcon.obj" : $(SOURCE) $(DEP_CPP_BITCO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\bitcon.obj"	"$(INTDIR)\bitcon.sbr" : $(SOURCE) $(DEP_CPP_BITCO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Chario.c
DEP_CPP_CHARI=\
	"..\config.h"\
	"..\Winextrn.h"\
	
NODEP_CPP_CHARI=\
	"..\externs.h"\
	"..\forth.h"\
	"..\protos.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Chario.obj" : $(SOURCE) $(DEP_CPP_CHARI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Chario.obj"	"$(INTDIR)\Chario.sbr" : $(SOURCE) $(DEP_CPP_CHARI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Cyberspc.c
DEP_CPP_CYBER=\
	"..\..\Cyberspc.h"\
	"..\..\externs.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\mememat.h"\
	"..\..\protos.h"\
	"..\..\rfile.h"\
	"..\..\sernum.h"\
	{$(INCLUDE)}"sys\timeb.h"\
	
NODEP_CPP_CYBER=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Cyberspc.obj" : $(SOURCE) $(DEP_CPP_CYBER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Cyberspc.obj"	"$(INTDIR)\Cyberspc.sbr" : $(SOURCE) $(DEP_CPP_CYBER)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\debuglog.c
DEP_CPP_DEBUG=\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\timeb.h"\
	{$(INCLUDE)}"sys\types.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\debuglog.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\debuglog.obj"	"$(INTDIR)\debuglog.sbr" : $(SOURCE) $(DEP_CPP_DEBUG)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Decser.c
DEP_CPP_DECSE=\
	"..\..\forth.h"\
	"..\..\sernum.h"\
	
NODEP_CPP_DECSE=\
	"..\..\config.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Decser.obj" : $(SOURCE) $(DEP_CPP_DECSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Decser.obj"	"$(INTDIR)\Decser.sbr" : $(SOURCE) $(DEP_CPP_DECSE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\dkmodule.c
DEP_CPP_DKMOD=\
	"..\config.h"\
	"..\Winextrn.h"\
	
NODEP_CPP_DKMOD=\
	"..\errors.h"\
	"..\externs.h"\
	"..\forth.h"\
	"..\meme.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\dkmodule.obj" : $(SOURCE) $(DEP_CPP_DKMOD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\dkmodule.obj"	"$(INTDIR)\dkmodule.sbr" : $(SOURCE) $(DEP_CPP_DKMOD)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\expiry.c
DEP_CPP_EXPIR=\
	"..\config.h"\
	
NODEP_CPP_EXPIR=\
	"..\externs.h"\
	"..\forth.h"\
	"..\protos.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\expiry.obj" : $(SOURCE) $(DEP_CPP_EXPIR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\expiry.obj"	"$(INTDIR)\expiry.sbr" : $(SOURCE) $(DEP_CPP_EXPIR)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Extend.c
DEP_CPP_EXTEN=\
	"..\config.h"\
	
NODEP_CPP_EXTEN=\
	"..\forth.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Extend.obj" : $(SOURCE) $(DEP_CPP_EXTEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Extend.obj"	"$(INTDIR)\Extend.sbr" : $(SOURCE) $(DEP_CPP_EXTEN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Forth.c
DEP_CPP_FORTH=\
	"..\..\Cyberspc.h"\
	"..\..\errors.h"\
	"..\..\externs.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\mememat.h"\
	"..\..\MEMEOPS.H"\
	"..\..\MEMEVER.H"\
	"..\..\prims.h"\
	"..\..\protos.h"\
	"..\..\rfile.h"\
	"..\..\sernum.h"\
	"..\..\SEROPS.H"\
	{$(INCLUDE)}"sys\timeb.h"\
	
NODEP_CPP_FORTH=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Forth.obj" : $(SOURCE) $(DEP_CPP_FORTH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Forth.obj"	"$(INTDIR)\Forth.sbr" : $(SOURCE) $(DEP_CPP_FORTH)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Globals.c
DEP_CPP_GLOBA=\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\MEMEVER.H"\
	"..\..\sernum.h"\
	
NODEP_CPP_GLOBA=\
	"..\..\config.h"\
	"..\..\memerend.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Globals.obj"	"$(INTDIR)\Globals.sbr" : $(SOURCE) $(DEP_CPP_GLOBA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Init.c
DEP_CPP_INIT_=\
	"..\config.h"\
	"..\psprotos.h"\
	"..\Winextrn.h"\
	
NODEP_CPP_INIT_=\
	"..\externs.h"\
	"..\forth.h"\
	"..\prims.h"\
	"..\protos.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Init.obj"	"$(INTDIR)\Init.sbr" : $(SOURCE) $(DEP_CPP_INIT_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Io.c
DEP_CPP_IO_C18=\
	"..\config.h"\
	"..\Winextrn.h"\
	
NODEP_CPP_IO_C18=\
	"..\ascii.h"\
	"..\externs.h"\
	"..\forth.h"\
	"..\prims.h"\
	"..\protos.h"\
	"..\sernum.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Io.obj" : $(SOURCE) $(DEP_CPP_IO_C18) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Io.obj"	"$(INTDIR)\Io.sbr" : $(SOURCE) $(DEP_CPP_IO_C18) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Main.c
DEP_CPP_MAIN_=\
	"..\config.h"\
	"..\psprotos.h"\
	"..\Winextrn.h"\
	
NODEP_CPP_MAIN_=\
	"..\ascii.h"\
	"..\externs.h"\
	"..\forth.h"\
	"..\protos.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Main.obj"	"$(INTDIR)\Main.sbr" : $(SOURCE) $(DEP_CPP_MAIN_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Matherr.c
DEP_CPP_MATHE=\
	"..\config.h"\
	
NODEP_CPP_MATHE=\
	"..\forth.h"\
	"..\meme.h"\
	"..\protos.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Matherr.obj" : $(SOURCE) $(DEP_CPP_MATHE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Matherr.obj"	"$(INTDIR)\Matherr.sbr" : $(SOURCE) $(DEP_CPP_MATHE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Meme.c
DEP_CPP_MEME_=\
	"..\..\Cyberspc.h"\
	"..\..\errors.h"\
	"..\..\externs.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\mememat.h"\
	"..\..\prims.h"\
	"..\..\protos.h"\
	"..\..\rfile.h"\
	"..\..\sernum.h"\
	
NODEP_CPP_MEME_=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Meme.obj" : $(SOURCE) $(DEP_CPP_MEME_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Meme.obj"	"$(INTDIR)\Meme.sbr" : $(SOURCE) $(DEP_CPP_MEME_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\MEME.rc
DEP_RSC_MEME_R=\
	"..\icon2.ico"\
	"..\icon6.ico"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\MEME.res" : $(SOURCE) $(DEP_RSC_MEME_R) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\MEME.res" /i "\meme\WINRWVC" /i\
 "C:\meme\WINRWVC" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\MEME.res" : $(SOURCE) $(DEP_RSC_MEME_R) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\MEME.res" /i "\meme\WINRWVC" /i\
 "C:\meme\WINRWVC" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\Memejoy.c
DEP_CPP_MEMEJ=\
	"..\config.h"\
	
NODEP_CPP_MEMEJ=\
	"..\forth.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Memejoy.obj" : $(SOURCE) $(DEP_CPP_MEMEJ) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Memejoy.obj"	"$(INTDIR)\Memejoy.sbr" : $(SOURCE) $(DEP_CPP_MEMEJ)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Mememat.c
DEP_CPP_MEMEM=\
	"..\..\Cyberspc.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\mememat.h"\
	"..\..\protos.h"\
	"..\..\rfile.h"\
	
NODEP_CPP_MEMEM=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Mememat.obj" : $(SOURCE) $(DEP_CPP_MEMEM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Mememat.obj"	"$(INTDIR)\Mememat.sbr" : $(SOURCE) $(DEP_CPP_MEMEM)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Mememaus.c
DEP_CPP_MEMEMA=\
	"..\config.h"\
	"..\Renderer.h"\
	"..\Winextrn.h"\
	
NODEP_CPP_MEMEMA=\
	"..\forth.h"\
	"..\rwlib.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Mememaus.obj" : $(SOURCE) $(DEP_CPP_MEMEMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Mememaus.obj"	"$(INTDIR)\Mememaus.sbr" : $(SOURCE) $(DEP_CPP_MEMEMA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\memenet.c
DEP_CPP_MEMEN=\
	"..\config.h"\
	"..\memenet.h"\
	"..\Winextrn.h"\
	{$(INCLUDE)}"sys\timeb.h"\
	
NODEP_CPP_MEMEN=\
	"..\errors.h"\
	"..\externs.h"\
	"..\forth.h"\
	"..\meme.h"\
	"..\messages.h"\
	"..\mstimer.h"\
	"..\prims.h"\
	"..\protos.h"\
	"..\rfile.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\memenet.obj" : $(SOURCE) $(DEP_CPP_MEMEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\memenet.obj"	"$(INTDIR)\memenet.sbr" : $(SOURCE) $(DEP_CPP_MEMEN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Memerend.c
DEP_CPP_MEMER=\
	"..\config.h"\
	"..\memerend.h"\
	"..\Renderer.h"\
	"..\Winextrn.h"\
	
NODEP_CPP_MEMER=\
	"..\errors.h"\
	"..\externs.h"\
	"..\forth.h"\
	"..\meme.h"\
	"..\mememat.h"\
	"..\protos.h"\
	"..\rwlib.h"\
	"..\rwwin.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Memerend.obj" : $(SOURCE) $(DEP_CPP_MEMER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Memerend.obj"	"$(INTDIR)\Memerend.sbr" : $(SOURCE) $(DEP_CPP_MEMER)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Memeser.c
DEP_CPP_MEMES=\
	"..\config.h"\
	
NODEP_CPP_MEMES=\
	"..\errors.h"\
	"..\externs.h"\
	"..\forth.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Memeser.obj" : $(SOURCE) $(DEP_CPP_MEMES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Memeser.obj"	"$(INTDIR)\Memeser.sbr" : $(SOURCE) $(DEP_CPP_MEMES)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Mstimer.c
DEP_CPP_MSTIM=\
	"..\..\mstimer.h"\
	{$(INCLUDE)}"sys\timeb.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Mstimer.obj" : $(SOURCE) $(DEP_CPP_MSTIM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Mstimer.obj"	"$(INTDIR)\Mstimer.sbr" : $(SOURCE) $(DEP_CPP_MSTIM)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Often.c
DEP_CPP_OFTEN=\
	"..\config.h"\
	"..\Winextrn.h"\
	
NODEP_CPP_OFTEN=\
	"..\externs.h"\
	"..\mstimer.h"\
	"..\protos.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Often.obj" : $(SOURCE) $(DEP_CPP_OFTEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Often.obj"	"$(INTDIR)\Often.sbr" : $(SOURCE) $(DEP_CPP_OFTEN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\rfcache.c
DEP_CPP_RFCAC=\
	"..\..\Cyberspc.h"\
	"..\..\externs.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\mememat.h"\
	"..\..\messages.h"\
	"..\..\protos.h"\
	"..\..\rfile.h"\
	"..\..\sernum.h"\
	
NODEP_CPP_RFCAC=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\rfcache.obj" : $(SOURCE) $(DEP_CPP_RFCAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\rfcache.obj"	"$(INTDIR)\rfcache.sbr" : $(SOURCE) $(DEP_CPP_RFCAC)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Rfile.c
DEP_CPP_RFILE=\
	"..\..\Cyberspc.h"\
	"..\..\externs.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\mememat.h"\
	"..\..\messages.h"\
	"..\..\protos.h"\
	"..\..\rfile.h"\
	"..\..\sernum.h"\
	
NODEP_CPP_RFILE=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Rfile.obj" : $(SOURCE) $(DEP_CPP_RFILE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Rfile.obj"	"$(INTDIR)\Rfile.sbr" : $(SOURCE) $(DEP_CPP_RFILE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Syscall.c
DEP_CPP_SYSCA=\
	"..\config.h"\
	
NODEP_CPP_SYSCA=\
	"..\externs.h"\
	"..\forth.h"\
	"..\protos.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Syscall.obj" : $(SOURCE) $(DEP_CPP_SYSCA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Syscall.obj"	"$(INTDIR)\Syscall.sbr" : $(SOURCE) $(DEP_CPP_SYSCA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Util.c
DEP_CPP_UTIL_=\
	"..\..\externs.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\sernum.h"\
	{$(INCLUDE)}"sys\timeb.h"\
	
NODEP_CPP_UTIL_=\
	"..\..\config.h"\
	"..\..\memerend.h"\
	

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Util.obj"	"$(INTDIR)\Util.sbr" : $(SOURCE) $(DEP_CPP_UTIL_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Winglobl.c

!IF  "$(CFG)" == "Memelib - Win32 Release"


"$(INTDIR)\Winglobl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Memelib - Win32 Debug"


"$(INTDIR)\Winglobl.obj"	"$(INTDIR)\Winglobl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

