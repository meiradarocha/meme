# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=NPWIN32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to NPWIN32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "NPWIN32 - Win32 Release" && "$(CFG)" !=\
 "NPWIN32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "NPWIN32.MAK" CFG="NPWIN32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NPWIN32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NPWIN32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "NPWIN32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\npmeme.dll" "$(OUTDIR)\WinRel\NPWIN32.bsc"

CLEAN : 
	-@erase ".\WinRel\NPWIN32.bsc"
	-@erase ".\WinRel\NPSHELL.SBR"
	-@erase ".\WinRel\npwin.sbr"
	-@erase ".\WinRel\UTIL.SBR"
	-@erase ".\WinRel\RFILE.SBR"
	-@erase ".\WinRel\rfcache.sbr"
	-@erase ".\WinRel\MSTIMER.SBR"
	-@erase ".\WinRel\MEMEMAT.SBR"
	-@erase ".\WinRel\MEME.SBR"
	-@erase ".\WinRel\GLOBALS.SBR"
	-@erase ".\WinRel\FORTH.SBR"
	-@erase ".\WinRel\DECSER.SBR"
	-@erase ".\WinRel\debuglog.sbr"
	-@erase ".\WinRel\CYBERSPC.SBR"
	-@erase ".\WinRel\CONFIG.SBR"
	-@erase ".\WinRel\WINGLOBL.SBR"
	-@erase ".\WinRel\SYSCALL.SBR"
	-@erase ".\WinRel\OFTEN.SBR"
	-@erase ".\WinRel\MEMESER.SBR"
	-@erase ".\WinRel\MEMEREND.SBR"
	-@erase ".\WinRel\memenet.sbr"
	-@erase ".\WinRel\MEMEMAUS.SBR"
	-@erase ".\WinRel\MEMEJOY.SBR"
	-@erase ".\WinRel\MEMEFILE.SBR"
	-@erase ".\WinRel\MATHERR.SBR"
	-@erase ".\WinRel\IO.SBR"
	-@erase ".\WinRel\INIT.SBR"
	-@erase ".\WinRel\EXTEND.SBR"
	-@erase ".\WinRel\expiry.sbr"
	-@erase ".\WinRel\dkmodule.sbr"
	-@erase ".\WinRel\CONSOLE.SBR"
	-@erase ".\WinRel\CHARIO.SBR"
	-@erase ".\WinRel\ALLOCDIC.SBR"
	-@erase ".\WinRel\MALLOCL.SBR"
	-@erase ".\npmeme.dll"
	-@erase ".\WinRel\NPSHELL.OBJ"
	-@erase ".\WinRel\npwin.obj"
	-@erase ".\WinRel\npdll32.res"
	-@erase ".\WinRel\UTIL.OBJ"
	-@erase ".\WinRel\RFILE.OBJ"
	-@erase ".\WinRel\rfcache.obj"
	-@erase ".\WinRel\MSTIMER.OBJ"
	-@erase ".\WinRel\MEMEMAT.OBJ"
	-@erase ".\WinRel\MEME.OBJ"
	-@erase ".\WinRel\GLOBALS.OBJ"
	-@erase ".\WinRel\FORTH.OBJ"
	-@erase ".\WinRel\DECSER.OBJ"
	-@erase ".\WinRel\debuglog.obj"
	-@erase ".\WinRel\CYBERSPC.OBJ"
	-@erase ".\WinRel\CONFIG.OBJ"
	-@erase ".\WinRel\WINGLOBL.OBJ"
	-@erase ".\WinRel\SYSCALL.OBJ"
	-@erase ".\WinRel\OFTEN.OBJ"
	-@erase ".\WinRel\MEMESER.OBJ"
	-@erase ".\WinRel\MEMEREND.OBJ"
	-@erase ".\WinRel\memenet.obj"
	-@erase ".\WinRel\MEMEMAUS.OBJ"
	-@erase ".\WinRel\MEMEJOY.OBJ"
	-@erase ".\WinRel\MEMEFILE.OBJ"
	-@erase ".\WinRel\MATHERR.OBJ"
	-@erase ".\WinRel\IO.OBJ"
	-@erase ".\WinRel\INIT.OBJ"
	-@erase ".\WinRel\EXTEND.OBJ"
	-@erase ".\WinRel\expiry.obj"
	-@erase ".\WinRel\dkmodule.obj"
	-@erase ".\WinRel\CONSOLE.OBJ"
	-@erase ".\WinRel\CHARIO.OBJ"
	-@erase ".\WinRel\ALLOCDIC.OBJ"
	-@erase ".\WinRel\MALLOCL.OBJ"
	-@erase ".\WinRel\npmeme.lib"
	-@erase ".\WinRel\npmeme.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "c:\meme" /I "c:\meme\winrwvc" /I "c:\rwwin20\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MEMEVIEW" /D "PLUGIN" /FR /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "c:\meme" /I "c:\meme\winrwvc" /I\
 "c:\rwwin20\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MEMEVIEW" /D\
 "PLUGIN" /FR"$(INTDIR)/" /Fp"$(INTDIR)/NPWIN32.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\WinRel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/npdll32.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NPWIN32.bsc" 
BSC32_SBRS= \
	".\WinRel\NPSHELL.SBR" \
	".\WinRel\npwin.sbr" \
	".\WinRel\UTIL.SBR" \
	".\WinRel\RFILE.SBR" \
	".\WinRel\rfcache.sbr" \
	".\WinRel\MSTIMER.SBR" \
	".\WinRel\MEMEMAT.SBR" \
	".\WinRel\MEME.SBR" \
	".\WinRel\GLOBALS.SBR" \
	".\WinRel\FORTH.SBR" \
	".\WinRel\DECSER.SBR" \
	".\WinRel\debuglog.sbr" \
	".\WinRel\CYBERSPC.SBR" \
	".\WinRel\CONFIG.SBR" \
	".\WinRel\WINGLOBL.SBR" \
	".\WinRel\SYSCALL.SBR" \
	".\WinRel\OFTEN.SBR" \
	".\WinRel\MEMESER.SBR" \
	".\WinRel\MEMEREND.SBR" \
	".\WinRel\memenet.sbr" \
	".\WinRel\MEMEMAUS.SBR" \
	".\WinRel\MEMEJOY.SBR" \
	".\WinRel\MEMEFILE.SBR" \
	".\WinRel\MATHERR.SBR" \
	".\WinRel\IO.SBR" \
	".\WinRel\INIT.SBR" \
	".\WinRel\EXTEND.SBR" \
	".\WinRel\expiry.sbr" \
	".\WinRel\dkmodule.sbr" \
	".\WinRel\CONSOLE.SBR" \
	".\WinRel\CHARIO.SBR" \
	".\WinRel\ALLOCDIC.SBR" \
	".\WinRel\MALLOCL.SBR"

"$(OUTDIR)\WinRel\NPWIN32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 c:\rwwin20\lib\rwl20.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:2.67 /subsystem:windows /dll /machine:I386 /out:"npmeme.dll"
LINK32_FLAGS=c:\rwwin20\lib\rwl20.lib wsock32.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:2.67\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/npmeme.pdb"\
 /machine:I386 /def:".\npdll32.def" /out:"npmeme.dll"\
 /implib:"$(OUTDIR)/npmeme.lib" 
DEF_FILE= \
	".\npdll32.def"
LINK32_OBJS= \
	".\WinRel\NPSHELL.OBJ" \
	".\WinRel\npwin.obj" \
	".\WinRel\UTIL.OBJ" \
	".\WinRel\RFILE.OBJ" \
	".\WinRel\rfcache.obj" \
	".\WinRel\MSTIMER.OBJ" \
	".\WinRel\MEMEMAT.OBJ" \
	".\WinRel\MEME.OBJ" \
	".\WinRel\GLOBALS.OBJ" \
	".\WinRel\FORTH.OBJ" \
	".\WinRel\DECSER.OBJ" \
	".\WinRel\debuglog.obj" \
	".\WinRel\CYBERSPC.OBJ" \
	".\WinRel\CONFIG.OBJ" \
	".\WinRel\WINGLOBL.OBJ" \
	".\WinRel\SYSCALL.OBJ" \
	".\WinRel\OFTEN.OBJ" \
	".\WinRel\MEMESER.OBJ" \
	".\WinRel\MEMEREND.OBJ" \
	".\WinRel\memenet.obj" \
	".\WinRel\MEMEMAUS.OBJ" \
	".\WinRel\MEMEJOY.OBJ" \
	".\WinRel\MEMEFILE.OBJ" \
	".\WinRel\MATHERR.OBJ" \
	".\WinRel\IO.OBJ" \
	".\WinRel\INIT.OBJ" \
	".\WinRel\EXTEND.OBJ" \
	".\WinRel\expiry.obj" \
	".\WinRel\dkmodule.obj" \
	".\WinRel\CONSOLE.OBJ" \
	".\WinRel\CHARIO.OBJ" \
	".\WinRel\ALLOCDIC.OBJ" \
	".\WinRel\MALLOCL.OBJ" \
	".\WinRel\npdll32.res"

"$(OUTDIR)\npmeme.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "NPWIN32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\npmeme.dll" "$(OUTDIR)\WinDebug\NPWIN32.bsc"

CLEAN : 
	-@erase ".\WinDebug\NPWIN32.bsc"
	-@erase ".\WinDebug\NPSHELL.SBR"
	-@erase ".\WinDebug\npwin.sbr"
	-@erase ".\WinDebug\UTIL.SBR"
	-@erase ".\WinDebug\RFILE.SBR"
	-@erase ".\WinDebug\rfcache.sbr"
	-@erase ".\WinDebug\MSTIMER.SBR"
	-@erase ".\WinDebug\MEMEMAT.SBR"
	-@erase ".\WinDebug\MEME.SBR"
	-@erase ".\WinDebug\GLOBALS.SBR"
	-@erase ".\WinDebug\FORTH.SBR"
	-@erase ".\WinDebug\DECSER.SBR"
	-@erase ".\WinDebug\debuglog.sbr"
	-@erase ".\WinDebug\CYBERSPC.SBR"
	-@erase ".\WinDebug\CONFIG.SBR"
	-@erase ".\WinDebug\WINGLOBL.SBR"
	-@erase ".\WinDebug\SYSCALL.SBR"
	-@erase ".\WinDebug\OFTEN.SBR"
	-@erase ".\WinDebug\MEMESER.SBR"
	-@erase ".\WinDebug\MEMEREND.SBR"
	-@erase ".\WinDebug\memenet.sbr"
	-@erase ".\WinDebug\MEMEMAUS.SBR"
	-@erase ".\WinDebug\MEMEJOY.SBR"
	-@erase ".\WinDebug\MEMEFILE.SBR"
	-@erase ".\WinDebug\MATHERR.SBR"
	-@erase ".\WinDebug\IO.SBR"
	-@erase ".\WinDebug\INIT.SBR"
	-@erase ".\WinDebug\EXTEND.SBR"
	-@erase ".\WinDebug\expiry.sbr"
	-@erase ".\WinDebug\dkmodule.sbr"
	-@erase ".\WinDebug\CONSOLE.SBR"
	-@erase ".\WinDebug\CHARIO.SBR"
	-@erase ".\WinDebug\ALLOCDIC.SBR"
	-@erase ".\WinDebug\MALLOCL.SBR"
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\vc40.idb"
	-@erase ".\npmeme.dll"
	-@erase ".\WinDebug\NPSHELL.OBJ"
	-@erase ".\WinDebug\npwin.obj"
	-@erase ".\WinDebug\npdll32.res"
	-@erase ".\WinDebug\UTIL.OBJ"
	-@erase ".\WinDebug\RFILE.OBJ"
	-@erase ".\WinDebug\rfcache.obj"
	-@erase ".\WinDebug\MSTIMER.OBJ"
	-@erase ".\WinDebug\MEMEMAT.OBJ"
	-@erase ".\WinDebug\MEME.OBJ"
	-@erase ".\WinDebug\GLOBALS.OBJ"
	-@erase ".\WinDebug\FORTH.OBJ"
	-@erase ".\WinDebug\DECSER.OBJ"
	-@erase ".\WinDebug\debuglog.obj"
	-@erase ".\WinDebug\CYBERSPC.OBJ"
	-@erase ".\WinDebug\CONFIG.OBJ"
	-@erase ".\WinDebug\WINGLOBL.OBJ"
	-@erase ".\WinDebug\SYSCALL.OBJ"
	-@erase ".\WinDebug\OFTEN.OBJ"
	-@erase ".\WinDebug\MEMESER.OBJ"
	-@erase ".\WinDebug\MEMEREND.OBJ"
	-@erase ".\WinDebug\memenet.obj"
	-@erase ".\WinDebug\MEMEMAUS.OBJ"
	-@erase ".\WinDebug\MEMEJOY.OBJ"
	-@erase ".\WinDebug\MEMEFILE.OBJ"
	-@erase ".\WinDebug\MATHERR.OBJ"
	-@erase ".\WinDebug\IO.OBJ"
	-@erase ".\WinDebug\INIT.OBJ"
	-@erase ".\WinDebug\EXTEND.OBJ"
	-@erase ".\WinDebug\expiry.obj"
	-@erase ".\WinDebug\dkmodule.obj"
	-@erase ".\WinDebug\CONSOLE.OBJ"
	-@erase ".\WinDebug\CHARIO.OBJ"
	-@erase ".\WinDebug\ALLOCDIC.OBJ"
	-@erase ".\WinDebug\MALLOCL.OBJ"
	-@erase ".\npmeme.ilk"
	-@erase ".\WinDebug\npmeme.lib"
	-@erase ".\WinDebug\npmeme.exp"
	-@erase ".\WinDebug\npmeme.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "c:\meme" /I "c:\meme\winrwvc" /I "c:\rwwin20\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MEMEVIEW" /D "PLUGIN" /FR /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "c:\meme" /I "c:\meme\winrwvc" /I\
 "c:\rwwin20\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MEMEVIEW" /D\
 "PLUGIN" /FR"$(INTDIR)/" /Fp"$(INTDIR)/NPWIN32.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\WinDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/npdll32.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NPWIN32.bsc" 
BSC32_SBRS= \
	".\WinDebug\NPSHELL.SBR" \
	".\WinDebug\npwin.sbr" \
	".\WinDebug\UTIL.SBR" \
	".\WinDebug\RFILE.SBR" \
	".\WinDebug\rfcache.sbr" \
	".\WinDebug\MSTIMER.SBR" \
	".\WinDebug\MEMEMAT.SBR" \
	".\WinDebug\MEME.SBR" \
	".\WinDebug\GLOBALS.SBR" \
	".\WinDebug\FORTH.SBR" \
	".\WinDebug\DECSER.SBR" \
	".\WinDebug\debuglog.sbr" \
	".\WinDebug\CYBERSPC.SBR" \
	".\WinDebug\CONFIG.SBR" \
	".\WinDebug\WINGLOBL.SBR" \
	".\WinDebug\SYSCALL.SBR" \
	".\WinDebug\OFTEN.SBR" \
	".\WinDebug\MEMESER.SBR" \
	".\WinDebug\MEMEREND.SBR" \
	".\WinDebug\memenet.sbr" \
	".\WinDebug\MEMEMAUS.SBR" \
	".\WinDebug\MEMEJOY.SBR" \
	".\WinDebug\MEMEFILE.SBR" \
	".\WinDebug\MATHERR.SBR" \
	".\WinDebug\IO.SBR" \
	".\WinDebug\INIT.SBR" \
	".\WinDebug\EXTEND.SBR" \
	".\WinDebug\expiry.sbr" \
	".\WinDebug\dkmodule.sbr" \
	".\WinDebug\CONSOLE.SBR" \
	".\WinDebug\CHARIO.SBR" \
	".\WinDebug\ALLOCDIC.SBR" \
	".\WinDebug\MALLOCL.SBR"

"$(OUTDIR)\WinDebug\NPWIN32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 c:\rwwin20\lib\rwl20.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:2.67 /subsystem:windows /dll /debug /machine:I386 /out:"npmeme.dll"
LINK32_FLAGS=c:\rwwin20\lib\rwl20.lib wsock32.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:2.67\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/npmeme.pdb" /debug\
 /machine:I386 /def:".\npdll32.def" /out:"npmeme.dll"\
 /implib:"$(OUTDIR)/npmeme.lib" 
DEF_FILE= \
	".\npdll32.def"
LINK32_OBJS= \
	".\WinDebug\NPSHELL.OBJ" \
	".\WinDebug\npwin.obj" \
	".\WinDebug\UTIL.OBJ" \
	".\WinDebug\RFILE.OBJ" \
	".\WinDebug\rfcache.obj" \
	".\WinDebug\MSTIMER.OBJ" \
	".\WinDebug\MEMEMAT.OBJ" \
	".\WinDebug\MEME.OBJ" \
	".\WinDebug\GLOBALS.OBJ" \
	".\WinDebug\FORTH.OBJ" \
	".\WinDebug\DECSER.OBJ" \
	".\WinDebug\debuglog.obj" \
	".\WinDebug\CYBERSPC.OBJ" \
	".\WinDebug\CONFIG.OBJ" \
	".\WinDebug\WINGLOBL.OBJ" \
	".\WinDebug\SYSCALL.OBJ" \
	".\WinDebug\OFTEN.OBJ" \
	".\WinDebug\MEMESER.OBJ" \
	".\WinDebug\MEMEREND.OBJ" \
	".\WinDebug\memenet.obj" \
	".\WinDebug\MEMEMAUS.OBJ" \
	".\WinDebug\MEMEJOY.OBJ" \
	".\WinDebug\MEMEFILE.OBJ" \
	".\WinDebug\MATHERR.OBJ" \
	".\WinDebug\IO.OBJ" \
	".\WinDebug\INIT.OBJ" \
	".\WinDebug\EXTEND.OBJ" \
	".\WinDebug\expiry.obj" \
	".\WinDebug\dkmodule.obj" \
	".\WinDebug\CONSOLE.OBJ" \
	".\WinDebug\CHARIO.OBJ" \
	".\WinDebug\ALLOCDIC.OBJ" \
	".\WinDebug\MALLOCL.OBJ" \
	".\WinDebug\npdll32.res"

"$(OUTDIR)\npmeme.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "NPWIN32 - Win32 Release"
# Name "NPWIN32 - Win32 Debug"

!IF  "$(CFG)" == "NPWIN32 - Win32 Release"

!ELSEIF  "$(CFG)" == "NPWIN32 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\NPSHELL.CPP
DEP_CPP_NPSHE=\
	".\npapi.h"\
	

!IF  "$(CFG)" == "NPWIN32 - Win32 Release"


"$(INTDIR)\NPSHELL.OBJ" : $(SOURCE) $(DEP_CPP_NPSHE) "$(INTDIR)"

"$(INTDIR)\NPSHELL.SBR" : $(SOURCE) $(DEP_CPP_NPSHE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "NPWIN32 - Win32 Debug"


"$(INTDIR)\NPSHELL.OBJ" : $(SOURCE) $(DEP_CPP_NPSHE) "$(INTDIR)"

"$(INTDIR)\NPSHELL.SBR" : $(SOURCE) $(DEP_CPP_NPSHE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\npdll32.rc
DEP_RSC_NPDLL=\
	".\res\npdll32.rc2"\
	

"$(INTDIR)\npdll32.res" : $(SOURCE) $(DEP_RSC_NPDLL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\npwin.cpp
DEP_CPP_NPWIN=\
	".\stdafx.h"\
	".\npapi.h"\
	".\npupp.h"\
	

"$(INTDIR)\npwin.obj" : $(SOURCE) $(DEP_CPP_NPWIN) "$(INTDIR)"

"$(INTDIR)\npwin.sbr" : $(SOURCE) $(DEP_CPP_NPWIN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\npdll32.def

!IF  "$(CFG)" == "NPWIN32 - Win32 Release"

!ELSEIF  "$(CFG)" == "NPWIN32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\UTIL.C
DEP_CPP_UTIL_=\
	{$(INCLUDE)}"\sys\timeb.h"\
	"..\..\forth.h"\
	"..\..\externs.h"\
	"..\..\meme.h"\
	"..\..\sernum.h"\
	
NODEP_CPP_UTIL_=\
	"..\..\config.h"\
	"..\..\memerend.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\UTIL.OBJ" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\UTIL.SBR" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\RFILE.C
DEP_CPP_RFILE=\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\messages.h"\
	"..\..\rfile.h"\
	"..\..\externs.h"\
	"..\..\protos.h"\
	"..\..\sernum.h"\
	"..\..\cyberspc.h"\
	"..\..\mememat.h"\
	
NODEP_CPP_RFILE=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\RFILE.OBJ" : $(SOURCE) $(DEP_CPP_RFILE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\RFILE.SBR" : $(SOURCE) $(DEP_CPP_RFILE) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\rfcache.c
DEP_CPP_RFCAC=\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\messages.h"\
	"..\..\rfile.h"\
	"..\..\externs.h"\
	"..\..\protos.h"\
	"..\..\sernum.h"\
	"..\..\cyberspc.h"\
	"..\..\mememat.h"\
	
NODEP_CPP_RFCAC=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\rfcache.obj" : $(SOURCE) $(DEP_CPP_RFCAC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\rfcache.sbr" : $(SOURCE) $(DEP_CPP_RFCAC) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\MSTIMER.C
DEP_CPP_MSTIM=\
	{$(INCLUDE)}"\sys\timeb.h"\
	"..\..\mstimer.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MSTIMER.OBJ" : $(SOURCE) $(DEP_CPP_MSTIM) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MSTIMER.SBR" : $(SOURCE) $(DEP_CPP_MSTIM) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\MEMEMAT.C
DEP_CPP_MEMEM=\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\mememat.h"\
	"..\..\protos.h"\
	"..\..\cyberspc.h"\
	"..\..\rfile.h"\
	
NODEP_CPP_MEMEM=\
	"..\..\config.h"\
	"..\..\memerend.h"\
	"..\..\memenet.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MEMEMAT.OBJ" : $(SOURCE) $(DEP_CPP_MEMEM) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MEMEMAT.SBR" : $(SOURCE) $(DEP_CPP_MEMEM) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\MEME.C
DEP_CPP_MEME_=\
	"..\..\forth.h"\
	"..\..\prims.h"\
	"..\..\meme.h"\
	"..\..\externs.h"\
	"..\..\protos.h"\
	"..\..\errors.h"\
	"..\..\sernum.h"\
	"..\..\cyberspc.h"\
	"..\..\mememat.h"\
	"..\..\rfile.h"\
	
NODEP_CPP_MEME_=\
	"..\..\config.h"\
	"..\..\memerend.h"\
	"..\..\memenet.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MEME.OBJ" : $(SOURCE) $(DEP_CPP_MEME_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MEME.SBR" : $(SOURCE) $(DEP_CPP_MEME_) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\GLOBALS.C
DEP_CPP_GLOBA=\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\memever.h"\
	"..\..\sernum.h"\
	
NODEP_CPP_GLOBA=\
	"..\..\config.h"\
	"..\..\memerend.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\GLOBALS.OBJ" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GLOBALS.SBR" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\FORTH.C
DEP_CPP_FORTH=\
	{$(INCLUDE)}"\sys\timeb.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\prims.h"\
	"..\..\memever.h"\
	"..\..\memeops.h"\
	"..\..\serops.h"\
	"..\..\externs.h"\
	"..\..\protos.h"\
	"..\..\cyberspc.h"\
	"..\..\rfile.h"\
	"..\..\errors.h"\
	"..\..\sernum.h"\
	"..\..\mememat.h"\
	
NODEP_CPP_FORTH=\
	"..\..\config.h"\
	"..\..\memerend.h"\
	"..\..\memenet.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\FORTH.OBJ" : $(SOURCE) $(DEP_CPP_FORTH) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\FORTH.SBR" : $(SOURCE) $(DEP_CPP_FORTH) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\DECSER.C
DEP_CPP_DECSE=\
	"..\..\sernum.h"\
	"..\..\forth.h"\
	
NODEP_CPP_DECSE=\
	"..\..\config.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\DECSER.OBJ" : $(SOURCE) $(DEP_CPP_DECSE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\DECSER.SBR" : $(SOURCE) $(DEP_CPP_DECSE) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\debuglog.c
DEP_CPP_DEBUG=\
	{$(INCLUDE)}"\sys\timeb.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\debuglog.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\debuglog.sbr" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\CYBERSPC.C
DEP_CPP_CYBER=\
	{$(INCLUDE)}"\sys\timeb.h"\
	"..\..\forth.h"\
	"..\..\cyberspc.h"\
	"..\..\externs.h"\
	"..\..\protos.h"\
	"..\..\meme.h"\
	"..\..\sernum.h"\
	"..\..\mememat.h"\
	"..\..\rfile.h"\
	
NODEP_CPP_CYBER=\
	"..\..\config.h"\
	"..\..\memenet.h"\
	"..\..\memerend.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\CYBERSPC.OBJ" : $(SOURCE) $(DEP_CPP_CYBER) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\CYBERSPC.SBR" : $(SOURCE) $(DEP_CPP_CYBER) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\CONFIG.C
DEP_CPP_CONFI=\
	"..\..\sernum.h"\
	"..\..\forth.h"\
	
NODEP_CPP_CONFI=\
	"..\..\config.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\CONFIG.OBJ" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\CONFIG.SBR" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\WINGLOBL.C

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\WINGLOBL.OBJ" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\WINGLOBL.SBR" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\SYSCALL.C
DEP_CPP_SYSCA=\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_SYSCA=\
	"..\forth.h"\
	"..\externs.h"\
	"..\protos.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\SYSCALL.OBJ" : $(SOURCE) $(DEP_CPP_SYSCA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\SYSCALL.SBR" : $(SOURCE) $(DEP_CPP_SYSCA) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\OFTEN.C
DEP_CPP_OFTEN=\
	"c:\meme\winrwvc\config.h"\
	"..\winextrn.h"\
	
NODEP_CPP_OFTEN=\
	"..\externs.h"\
	"..\mstimer.h"\
	"..\protos.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\OFTEN.OBJ" : $(SOURCE) $(DEP_CPP_OFTEN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\OFTEN.SBR" : $(SOURCE) $(DEP_CPP_OFTEN) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\MEMESER.C
DEP_CPP_MEMES=\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_MEMES=\
	"..\forth.h"\
	"..\externs.h"\
	"..\errors.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MEMESER.OBJ" : $(SOURCE) $(DEP_CPP_MEMES) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MEMESER.SBR" : $(SOURCE) $(DEP_CPP_MEMES) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\MEMEREND.C
DEP_CPP_MEMER=\
	"..\renderer.h"\
	"c:\meme\winrwvc\config.h"\
	"..\winextrn.h"\
	"..\memerend.h"\
	
NODEP_CPP_MEMER=\
	"..\rwlib.h"\
	"..\rwwin.h"\
	"..\forth.h"\
	"..\externs.h"\
	"..\meme.h"\
	"..\mememat.h"\
	"..\protos.h"\
	"..\errors.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MEMEREND.OBJ" : $(SOURCE) $(DEP_CPP_MEMER) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MEMEREND.SBR" : $(SOURCE) $(DEP_CPP_MEMER) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\memenet.c
DEP_CPP_MEMEN=\
	{$(INCLUDE)}"\sys\timeb.h"\
	"c:\meme\winrwvc\config.h"\
	"..\memenet.h"\
	"..\winextrn.h"\
	
NODEP_CPP_MEMEN=\
	"..\forth.h"\
	"..\externs.h"\
	"..\errors.h"\
	"..\prims.h"\
	"..\messages.h"\
	"..\rfile.h"\
	"..\protos.h"\
	"..\mstimer.h"\
	"..\meme.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\memenet.obj" : $(SOURCE) $(DEP_CPP_MEMEN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\memenet.sbr" : $(SOURCE) $(DEP_CPP_MEMEN) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\MEMEMAUS.C
DEP_CPP_MEMEMA=\
	"..\renderer.h"\
	"c:\meme\winrwvc\config.h"\
	"..\winextrn.h"\
	
NODEP_CPP_MEMEMA=\
	"..\rwlib.h"\
	"..\forth.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MEMEMAUS.OBJ" : $(SOURCE) $(DEP_CPP_MEMEMA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MEMEMAUS.SBR" : $(SOURCE) $(DEP_CPP_MEMEMA) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\MEMEJOY.C
DEP_CPP_MEMEJ=\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_MEMEJ=\
	"..\forth.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MEMEJOY.OBJ" : $(SOURCE) $(DEP_CPP_MEMEJ) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MEMEJOY.SBR" : $(SOURCE) $(DEP_CPP_MEMEJ) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\MEMEFILE.C
DEP_CPP_MEMEF=\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_MEMEF=\
	"..\forth.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MEMEFILE.OBJ" : $(SOURCE) $(DEP_CPP_MEMEF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MEMEFILE.SBR" : $(SOURCE) $(DEP_CPP_MEMEF) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\MATHERR.C
DEP_CPP_MATHE=\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_MATHE=\
	"..\forth.h"\
	"..\meme.h"\
	"..\protos.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MATHERR.OBJ" : $(SOURCE) $(DEP_CPP_MATHE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MATHERR.SBR" : $(SOURCE) $(DEP_CPP_MATHE) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\IO.C
DEP_CPP_IO_C1d=\
	"c:\meme\winrwvc\config.h"\
	"..\winextrn.h"\
	
NODEP_CPP_IO_C1d=\
	"..\forth.h"\
	"..\externs.h"\
	"..\protos.h"\
	"..\sernum.h"\
	"..\prims.h"\
	"..\ascii.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\IO.OBJ" : $(SOURCE) $(DEP_CPP_IO_C1d) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\IO.SBR" : $(SOURCE) $(DEP_CPP_IO_C1d) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\INIT.C
DEP_CPP_INIT_=\
	"c:\meme\winrwvc\config.h"\
	"..\winextrn.h"\
	"..\psprotos.h"\
	
NODEP_CPP_INIT_=\
	"..\forth.h"\
	"..\prims.h"\
	"..\externs.h"\
	"..\protos.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\INIT.OBJ" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\INIT.SBR" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\EXTEND.C
DEP_CPP_EXTEN=\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_EXTEN=\
	"..\forth.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\EXTEND.OBJ" : $(SOURCE) $(DEP_CPP_EXTEN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\EXTEND.SBR" : $(SOURCE) $(DEP_CPP_EXTEN) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\expiry.c
DEP_CPP_EXPIR=\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_EXPIR=\
	"..\forth.h"\
	"..\externs.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\expiry.obj" : $(SOURCE) $(DEP_CPP_EXPIR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\expiry.sbr" : $(SOURCE) $(DEP_CPP_EXPIR) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\dkmodule.c
DEP_CPP_DKMOD=\
	"c:\meme\winrwvc\config.h"\
	"..\winextrn.h"\
	
NODEP_CPP_DKMOD=\
	"..\forth.h"\
	"..\meme.h"\
	"..\externs.h"\
	"..\errors.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dkmodule.obj" : $(SOURCE) $(DEP_CPP_DKMOD) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dkmodule.sbr" : $(SOURCE) $(DEP_CPP_DKMOD) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\CONSOLE.C
DEP_CPP_CONSO=\
	"c:\meme\winrwvc\config.h"\
	"..\winextrn.h"\
	
NODEP_CPP_CONSO=\
	"..\forth.h"\
	"..\externs.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\CONSOLE.OBJ" : $(SOURCE) $(DEP_CPP_CONSO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\CONSOLE.SBR" : $(SOURCE) $(DEP_CPP_CONSO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\CHARIO.C
DEP_CPP_CHARI=\
	"..\winextrn.h"\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_CHARI=\
	"..\forth.h"\
	"..\externs.h"\
	"..\protos.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\CHARIO.OBJ" : $(SOURCE) $(DEP_CPP_CHARI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\CHARIO.SBR" : $(SOURCE) $(DEP_CPP_CHARI) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\WINRWVC\ALLOCDIC.C
DEP_CPP_ALLOC=\
	"c:\meme\winrwvc\config.h"\
	
NODEP_CPP_ALLOC=\
	"..\forth.h"\
	"..\externs.h"\
	"..\protos.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ALLOCDIC.OBJ" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ALLOCDIC.SBR" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=\meme\MALLOCL.C
DEP_CPP_MALLO=\
	"c:\meme\winrwvc\config.h"\
	"..\..\forth.h"\
	"..\..\meme.h"\
	"..\..\protos.h"\
	"..\..\cyberspc.h"\
	"..\..\mememat.h"\
	"..\..\rfile.h"\
	
NODEP_CPP_MALLO=\
	"..\..\memerend.h"\
	"..\..\memenet.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\MALLOCL.OBJ" : $(SOURCE) $(DEP_CPP_MALLO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\MALLOCL.SBR" : $(SOURCE) $(DEP_CPP_MALLO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
# End Target
# End Project
################################################################################
