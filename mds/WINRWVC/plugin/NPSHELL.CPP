//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
// npshell.cpp
//
// This file defines a "shell" plugin that plugin developers can use
// as the basis for a real plugin.  This shell just provides empty
// implementations of all functions that the plugin can implement
// that will be called by Netscape (the NPP_xxx methods defined in 
// npapi.h). 
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#include "windows.h"

#ifndef WIN32
    #include "string.h"
#endif

#ifndef _NPAPI_H_
#include "npapi.h"
#endif

#define BACKGROUND_COLOR RGB(192, 192, 192)

//
// Instance state information about the plugin.
//
// *Developers*: Use this struct to hold per-instance
//               information that you'll need in the
//               various functions in this file.
//

typedef struct _PluginInstance PluginInstance;
typedef struct _PluginInstance
{
    NPWindow*       fWindow;
    HWND            hWnd;
    uint16          fMode;
    FARPROC         lpfnOldWndProc;
    NPSavedData*    pSavedInstanceData;
    PluginInstance* pNext;
} PluginInstance;

LONG NP_LOADDS WINAPI 
SubClassFunc(HWND hWnd,WORD Message,WORD wParam, LONG lParam);

HINSTANCE DLLInstance;

#ifdef WIN32

BOOL WINAPI
DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
    DLLInstance = hDLL;
    return 1;
}

#else

int CALLBACK
LibMain(HINSTANCE hinst, WORD wDataSeg, WORD cbHeap, LPSTR lpszCmdLine)
{
    DLLInstance = hinst;
    return 1;
}

#endif

// A plugin instance typically will subclass the plugin's client window, so
// it can get Windows messages, (such as paint, palettechanged, keybd, etc).
// To do work associated with a specific plugin instance the WndProc which
// receives the Windows messages, (named "SubClassFunc" herein), needs access
// to the "This" (PluginInstance*) ptr.

// When Navigator registers the plugin client's window class, (the class for
// the window passed in NPP_SetWindow()), Navigator does not reserve any
// "extra" windows bytes.  If it had, the plugin could simply have stored its
// "This" (PluginInstance*) ptr in the extra bytes.  But Nav did not, and the
// plugin cannot, so a different technique must be used.  The technique used
// is to keep a linked list of PluginInstance structures, and walk the list
// to find which one is associated with the window handle.  Inefficient,
// grungy, complicates the code, etc.  C'est la vie ...

PluginInstance* g_pHeadInstanceList = NULL; 

// Associate the hWnd with pInstance by setting the hWnd member of the
// PluginInstance struct.  Also, add the PluginInstance struct to the list
// if necessary
static void AssociateInstance(HWND hWnd, PluginInstance* pInstance)
{
    pInstance->hWnd = hWnd;

    // add this PluginInstance to the list if it's not already
    if(g_pHeadInstanceList != NULL) { // anything in the list?
        if(g_pHeadInstanceList != pInstance) { // its not first in the list
            PluginInstance* pInst = g_pHeadInstanceList;
            while(pInst->pNext != NULL) {
                if(pInst->pNext == pInstance)
                    return; // found it, done
                pInst = pInst->pNext;
            }
            // didn't find it, add it
            pInst->pNext = pInstance;
        }
    }
    else // list is empty, just add it
        g_pHeadInstanceList = pInstance;
}

// Find the PluginInstance associated with this hWnd and return it
static PluginInstance* GetInstance(HWND hWnd)
{
    for(PluginInstance* pInstance = g_pHeadInstanceList;
        pInstance != NULL;
        pInstance = pInstance->pNext) {
            if(pInstance->hWnd == hWnd)
                return pInstance; // found it, done
    }
    return NULL;
}


//----------------------------------------------------------------------------
// NPP_Initialize:
//----------------------------------------------------------------------------
NPError NPP_Initialize(void)
{
    // do your one time initialization here, such as dynamically loading
    // dependant DLLs
    return NPERR_NO_ERROR;
}


//----------------------------------------------------------------------------
// NPP_Shutdown:
//----------------------------------------------------------------------------
void NPP_Shutdown(void)
{
    // do your one time uninitialization here, such as unloading dynamically
    // loaded DLLs
}


//----------------------------------------------------------------------------
// NPP_New:
//----------------------------------------------------------------------------
NPError NP_LOADDS
NPP_New(NPMIMEType pluginType,
                NPP instance,
                uint16 mode,
                int16 argc,
                char* argn[],
                char* argv[],
                NPSavedData* saved)
{
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;
        
    instance->pdata = NPN_MemAlloc(sizeof(PluginInstance));
    PluginInstance* This = (PluginInstance*) instance->pdata;

    if (This == NULL)
        return NPERR_OUT_OF_MEMORY_ERROR;
    //
    // *Developers*: Initialize fields of your plugin
    // instance data here.  If the NPSavedData is non-
    // NULL, you can use that data (returned by you from
    // NPP_Destroy to set up the new plugin instance.
    //
        
    This->fWindow = NULL;
    // mode is NP_EMBED, NP_FULL, or NP_BACKGROUND (see npapi.h)
    This->fMode = mode;
    This->hWnd = NULL;
    This->pSavedInstanceData = saved;
    This->pNext = NULL;

    return NPERR_NO_ERROR;
}


//-----------------------------------------------------------------------------
// NPP_Destroy:
//-----------------------------------------------------------------------------
NPError NP_LOADDS
NPP_Destroy(NPP instance, NPSavedData** save)
{
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    PluginInstance* This = (PluginInstance*) instance->pdata;

    //
    // *Developers*: If desired, call NP_MemAlloc to create a
    // NPSavedDate structure containing any state information
    // that you want restored if this plugin instance is later
    // recreated.
    //

    if (This != NULL)
    {
        // Remove the subclass for the client window
        if(This->hWnd) {
            SetWindowLong(This->hWnd,
                        GWL_WNDPROC,
                        (DWORD)This->lpfnOldWndProc);
        }

        // Remove this PluginInstance from the list
        if(g_pHeadInstanceList != NULL) { // anything in the list?
            if(g_pHeadInstanceList == This) {
            // handle the head of the list here to simplify the for loop below
                g_pHeadInstanceList = This->pNext;
            }
            else {
                for(PluginInstance* pInstance = g_pHeadInstanceList;
                    pInstance != NULL;
                    pInstance = pInstance->pNext) {
                        if(pInstance->pNext == This) {
                            pInstance->pNext = This->pNext;
                            break; // done walking list
                        }
                }
            }
        }

        NPN_MemFree(instance->pdata);
        instance->pdata = NULL;

        // make some saved instance data if necessary
        if(This->pSavedInstanceData == NULL) {
            // make a struct header for the data
            This->pSavedInstanceData =
                (NPSavedData*)NPN_MemAlloc(sizeof NPSavedData);

            // fill in the struct
            if(This->pSavedInstanceData != NULL) {
                This->pSavedInstanceData->len = 0;
                This->pSavedInstanceData->buf = NULL;

                // replace the def below and references to it with your data
                #define SIDATA "aSavedInstanceDataBlock"

                // the data
                This->pSavedInstanceData->buf = NPN_MemAlloc(sizeof SIDATA);

                if(This->pSavedInstanceData->buf != NULL) {
                    strcpy((char*)This->pSavedInstanceData->buf, SIDATA);
                    This->pSavedInstanceData->len = sizeof SIDATA;
                }
            }
        }

        // save some instance data
        *save = This->pSavedInstanceData;
    }

    return NPERR_NO_ERROR;
}


//----------------------------------------------------------------------------
// NPP_SetWindow:
//----------------------------------------------------------------------------
NPError NP_LOADDS
NPP_SetWindow(NPP instance, NPWindow* window)
{
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    PluginInstance* This = (PluginInstance*) instance->pdata;

    //
    // *Developers*: Before setting fWindow to point to the
    // new window, you may wish to compare the new window
    // info to the previous window (if any) to note window
    // size changes, etc.
    //
    if((window->window != NULL) && (This->hWnd == NULL))
    {
        This->fWindow = window;
        This->hWnd    = (HWND)(DWORD)This->fWindow->window;

        // subclass the window
        This->lpfnOldWndProc =
            (FARPROC)SetWindowLong(This->hWnd,
                                   GWL_WNDPROC,
                                   (DWORD)SubClassFunc);
        AssociateInstance(This->hWnd, This);
    }
    else {
        // if window handle changed
        if(This->hWnd != (HWND)(DWORD)window->window) {
            // remember the new window
            This->fWindow = window;

            // Remove the subclass for the old client window
            SetWindowLong(This->hWnd,
                        GWL_WNDPROC,
                        (DWORD)This->lpfnOldWndProc);

            // remember the new window handle
            This->hWnd = (HWND)(DWORD)This->fWindow->window;

            if(This->hWnd != NULL) {
                // subclass the new one
                This->lpfnOldWndProc =
                    (FARPROC)SetWindowLong(This->hWnd,
                                           GWL_WNDPROC,
                                           (DWORD)SubClassFunc);
                AssociateInstance(This->hWnd, This);
            }
        }
    }

    // the window position/size may have changed
    if(This->hWnd != NULL) {
        BOOL ret = SetWindowPos(This->hWnd,
                       NULL,
                       (int)This->fWindow->x,
                       (int)This->fWindow->y,
                       (int)This->fWindow->width,
                       (int)This->fWindow->height,
                       SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(This->hWnd, NULL, TRUE);
        UpdateWindow(This->hWnd);
    }

    return NPERR_NO_ERROR;
}


//----------------------------------------------------------------------------
// NPP_NewStream:
//----------------------------------------------------------------------------
NPError NP_LOADDS
NPP_NewStream(NPP instance,
              NPMIMEType type,
              NPStream *stream, 
              NPBool seekable,
              uint16 *stype)
{
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;
    PluginInstance* This = (PluginInstance*) instance->pdata;
    
    // if your plugin must operate file based, you may wish to do this:
    //    *stype = NP_ASFILE;
    // remember, though, that use of NP_ASFILE is strongly discouraged;
    // your plugin should attempt to work with data as it comes in on
    // the stream if at all possible

    return NPERR_NO_ERROR;
}


//
// *Developers*: 
// These next 2 functions are directly relevant in a plug-in which handles the
// data in a streaming manner.  If you want zero bytes because no buffer space
// is YET available, return 0.  As long as the stream has not been written
// to the plugin, Navigator will continue trying to send bytes.  If the plugin
// doesn't want them, just return some large number from NPP_WriteReady(), and
// ignore them in NPP_Write().  For a NP_ASFILE stream, they are still called
// but can safely be ignored using this strategy.
//

int32 STREAMBUFSIZE = 0X0FFFFFFF;   // If we are reading from a file in
                                    // NP_ASFILE mode, we can take any size
                                    // stream in our write call (since we
                                    // ignore it)

//----------------------------------------------------------------------------
// NPP_WriteReady:
//----------------------------------------------------------------------------
int32 NP_LOADDS
NPP_WriteReady(NPP instance, NPStream *stream)
{
    if (instance != NULL)
        PluginInstance* This = (PluginInstance*) instance->pdata;

    return STREAMBUFSIZE;   // Number of bytes ready to accept in NPP_Write()
}


//----------------------------------------------------------------------------
// NPP_Write:
//----------------------------------------------------------------------------
int32 NP_LOADDS
NPP_Write(NPP instance, NPStream *stream,
          int32 offset, int32 len, void *buffer)
{
    if (instance != NULL)
        PluginInstance* This = (PluginInstance*) instance->pdata;

    return len;     // The number of bytes accepted.  Return a
                    // negative number here if, e.g., there was an error
                    // during plugin operation and you want to abort the
                    // stream
}


//----------------------------------------------------------------------------
// NPP_DestroyStream:
//----------------------------------------------------------------------------
NPError NP_LOADDS
NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;
    PluginInstance* This = (PluginInstance*) instance->pdata;

    return NPERR_NO_ERROR;
}


//----------------------------------------------------------------------------
// NPP_StreamAsFile:
//----------------------------------------------------------------------------
void NP_LOADDS
NPP_StreamAsFile(NPP instance, NPStream *stream, const char* fname)
{
    if (instance == NULL)
        return;

    PluginInstance* This = (PluginInstance*) instance->pdata;
}


//----------------------------------------------------------------------------
// NPP_Print:
//----------------------------------------------------------------------------
void NP_LOADDS
NPP_Print(NPP instance, NPPrint* printInfo)
{
    if(printInfo == NULL)   // trap invalid parm
        return;

    if (instance != NULL)
    {
        PluginInstance* This = (PluginInstance*) instance->pdata;
    
        if (printInfo->mode == NP_FULL)
        {
            //
            // *Developers*: If your plugin would like to take over
            // printing completely when it is in full-screen mode,
            // set printInfo->pluginPrinted to TRUE and print your
            // plugin as you see fit.  If your plugin wants Netscape
            // to handle printing in this case, set printInfo->pluginPrinted
            // to FALSE (the default) and do nothing.  If you do want
            // to handle printing yourself, printOne is true if the
            // print button (as opposed to the print menu) was clicked.
            // On the Macintosh, platformPrint is a THPrint; on Windows,
            // platformPrint is a structure (defined in npapi.h) containing
            // the printer name, port, etc.
            //
            void* platformPrint = printInfo->print.fullPrint.platformPrint;
            NPBool printOne = printInfo->print.fullPrint.printOne;
            
            printInfo->print.fullPrint.pluginPrinted = FALSE; // Do the default
        
        }
        else    // If not fullscreen, we must be embedded
        {
            //
            // *Developers*: If your plugin is embedded, or is full-screen
            // but you returned false in pluginPrinted above, NPP_Print
            // will be called with mode == NP_EMBED.  The NPWindow
            // in the printInfo gives the location and dimensions of
            // the embedded plugin on the printed page.  On the Macintosh,
            // platformPrint is the printer port; on Windows, platformPrint
            // is the handle to the printing device context.
            //
            NPWindow* printWindow = &(printInfo->print.embedPrint.window);
            void* platformPrint = printInfo->print.embedPrint.platformPrint;

            HPEN hPen, hPenOld;
#ifdef WIN32
            /* Initialize the pen's "brush" */
            LOGBRUSH lb;
            lb.lbStyle = BS_SOLID;
            lb.lbColor = RGB(128, 128, 128);
            lb.lbHatch = 0;

            hPen = ExtCreatePen(PS_COSMETIC | PS_SOLID, 1, &lb, 0, NULL);
#else
            COLORREF cref = RGB(128, 128, 128);
            hPen = CreatePen(PS_SOLID, 32, cref);
#endif
            HDC hDC = (HDC)(DWORD)platformPrint;
            hPenOld = (HPEN)SelectObject(hDC, hPen);
    
            BOOL result = Rectangle(hDC,
                                    (int)(printWindow->x),
                                    (int)(printWindow->y),
                                    (int)(printWindow->x + printWindow->width),
                                    (int)(printWindow->y + printWindow->height));
            SelectObject(hDC, hPenOld);
            DeleteObject(hPen);
        }
    }
}


//----------------------------------------------------------------------------
// NPP_HandleEvent:
// Mac-only.
//----------------------------------------------------------------------------
int16 NPP_HandleEvent(NPP instance, void* event)
{
    NPBool eventHandled = FALSE;
    if (instance == NULL)
        return eventHandled;
        
    PluginInstance* This = (PluginInstance*) instance->pdata;
    
    //
    // *Developers*: The "event" passed in is a Macintosh
    // EventRecord*.  The event.what field can be any of the
    // normal Mac event types, or one of the following additional
    // types defined in npapi.h: getFocusEvent, loseFocusEvent,
    // adjustCursorEvent.  The focus events inform your plugin
    // that it will become, or is no longer, the recepient of
    // key events.  If your plugin doesn't want to receive key
    // events, return false when passed at getFocusEvent.  The
    // adjustCursorEvent is passed repeatedly when the mouse is
    // over your plugin; if your plugin doesn't want to set the
    // cursor, return false.  Handle the standard Mac events as
    // normal.  The return value for all standard events is currently
    // ignored except for the key event: for key events, only return
    // true if your plugin has handled that particular key event. 
    //
    
    return eventHandled;
}

//
// Here is a sample subclass function.
//
LONG NP_LOADDS WINAPI 
SubClassFunc(  HWND hWnd,
               WORD Message,
               WORD wParam,
               LONG lParam)
{
    PluginInstance *This = GetInstance(hWnd);
                          
    switch(Message) {
    case WM_PALETTECHANGED:
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);    
        return 0;
        break;

    case WM_PAINT:
        {
        HBRUSH hBrush, hBrushOld;
        PAINTSTRUCT paint;
        HDC hDC = BeginPaint(hWnd, &paint);

        hBrush = CreateSolidBrush(BACKGROUND_COLOR);
        hBrushOld = (HPEN)SelectObject(hDC, hBrush);

        BOOL result = FillRect(hDC, &paint.rcPaint, hBrush);
        SelectObject(hDC, hBrushOld);
        DeleteObject(hBrush);
        EndPaint(hWnd, &paint);
        }
        return 0;
        break;

    default:
        break;
    }

    return CallWindowProc(This->lpfnOldWndProc,
                          hWnd,
                          Message,
                          wParam,
                          lParam);
}

