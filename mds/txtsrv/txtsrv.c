/* Portions of this file are copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

//
// Text Server Version 1.0, a Windows Sockets Server
//
// Copyright 1993, Lee Murach
//
// Permission to use, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appears in all copies and
// that both that copyright notice and this permission notice appear in
// supporting documentation.  Lee makes no claims as to the suitability
// of this software for any purpose.
//
// Module TXTSRV, the only module of the Text Server, (TS) is both the
// user interface and the network interface.  Since TS is a server, its
// 'users' are TS clients, so the user interface requirements are
// rather simple. TS simply updates its display with outbound buffer
// traffic.  We also display winsock errors.
//
// TS is not a concurrent server; it processes one client request at a
// time.  This simplifies the design considerably.  TS uses
// WSAAsyncSelect() to receive the messages that notify of pending
// client requests, and prompt TS to deliver a reply.  However, TS
// switches to synchronous operation when retrieving a request, or
// delivering a reply.  TS reenables request notification (FD_ACCEPT)
// once it has finished its reply.  Meanwhile, other connection
// requests remain in the listen() queue.
//
// TS speaks the finger protocol, and will reply to finger (e.g., the
// winsock finger 3.x client).  We take the lazy way out, and assume
// the text file has <CR><LF> terminators. (as DOS files do)
//
// 3/26/93  Lee Murach     wrote this thing.
//

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "winsock.h"
#include "txtsrv.h"

/* ZAP CODE BEGINS HERE */

int portnumber = 8787; /* Taken fron Thin Air */

struct list_item {
   struct list_item *next;
   struct in_addr ip;
};

struct list_of_lists
{
   struct list_of_lists *next;
   char *list_id;
   struct list_item *list;
} *list_of_list_head = NULL;

/* ZAP CODE ENDS HERE */

#define WSVERSION 0x101             // windows sockets version
#define SERVERPORT portnumber       // server listens on this port
#define DEFAULTFILE "default.txt"   // send this file for null queries
#define REQLEN 255                  // max length of request string
#define XFERBUFLEN 1024             // max # of character for send/recv
#define WNDPROC   FARPROC

typedef struct                      // associates messages (or menu ids) 
{                                   // with a handler function
   UINT Code;
   LONG (*Fxn)(HWND, UINT, UINT, LONG);
} DECODEWORD;

typedef struct                      // associates an error code with text
{
   UINT err;
   char *sztext;
} ERRENTRY;

#define dim(x) (sizeof(x) / sizeof(x[0]))

HWND hFrame;                        // handle of main window
HANDLE hInst;                       // our instance
char szAppName[] = "Metatopia(tm) Space Server"; // the server named text
SOCKET ListenSocket;                // awaits connection requests
SOCKET ConnectSocket;               // is connected to client
char Request[REQLEN + 1];           // holds request from client
int ReqLen;                         // length of request string
char Buf[XFERBUFLEN];               // transfer buffer holds in/outbound text
int BufLen = 0;                     // # of chars in xfer buffer

BOOL InitApp(HANDLE hInstance);
BOOL InitInstance(HANDLE hInstance, int nCmdShow);
BOOL FAR APIENTRY AboutDlgProc(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG FAR APIENTRY FrameWndProc(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoMenuAbout(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoPaint(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoCommand(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoListen(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoClose(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoDestroy(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoConnection(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoGetRequest(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
LONG DoReply(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
void SendError(char *errstr);
void SendString(char *str);
void SendList(char *listID);
void SendOne(char *listID, struct in_addr *addr);
void Blocking(SOCKET s);
void DebugOut(char FAR *lps, ...);
void DisplayWSError(void);
void CloseSockets(void);
void UpdateDisplay();
void SizeWindow();
char *WSErrorString(UINT err);

#define WM_LISTEN       (WM_USER + 1)  // listen for connections
#define WM_CONNECTION   (WM_USER + 2)  // connection request awaits
#define WM_GETREQUEST   (WM_USER + 3)  // request is waiting
#define WM_REPLY        (WM_USER + 4)  // start reply

DECODEWORD frameMsgs[] =               // windows messages & handlers
{
   WM_LISTEN,     DoListen,
   WM_CONNECTION, DoConnection,
   WM_GETREQUEST, DoGetRequest,
   WM_REPLY,      DoReply,
   WM_COMMAND,    DoCommand,
   WM_PAINT,      DoPaint,
   WM_CLOSE,      DoClose,
   WM_DESTROY,    DoDestroy
};

DECODEWORD menuItems[] =            // menu items & associated handlers
{
   IDM_ABOUT,     DoMenuAbout,
   0,             0
};

ERRENTRY WSErrors[] =               // error text for windows sockets errors
{
   WSAVERNOTSUPPORTED,  "This version of Windows Sockets is not supported",
   WSASYSNOTREADY,      "Windows Sockets is not present or is not responding",
};

//
// WinMain -- Windows calls this to start the application.
//
int APIENTRY WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
   WSADATA WSAData;                          // windows sockets info return
   MSG msg;                                  // holds current message
   int err;
   int port;

   port = atoi(lpCmdLine);

   if (port) 
      portnumber = port;
   
   hInst = hInstance;                        // save the instance handle
   if (hPrevInstance)                        // only one server, thank you
      return(FALSE);

   if (!(InitApp(hInstance) && InitInstance(hInstance, nCmdShow)))
      return(FALSE);                         // can't start, so bail

   if (err = WSAStartup(WSVERSION, &WSAData))// connect to winsock
   {
      MessageBox(hFrame, WSErrorString(err), szAppName, MB_ICONSTOP | MB_OK);
      DestroyWindow(hFrame);                 // kill application window &
   }                                         // signal app exit
   else
      PostMessage(hFrame, WM_LISTEN, 0, 0);  // get ready for clients
   
   while (GetMessage(&msg, NULL, 0, 0))      // loop til WM_QUIT
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   WSACleanup();                             // disconnect from winsock
   return(msg.wParam);                       // return to windows
}

//
// InitApp -- initialization for all instances of application.
// This registers the main window class.
//
BOOL InitApp(HANDLE hInstance)
{
   WNDCLASS    wndclass;

   wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
   wndclass.lpfnWndProc   = FrameWndProc;
   wndclass.cbClsExtra    = 0;
   wndclass.cbWndExtra    = 0;
   wndclass.hInstance     = hInstance;
   wndclass.hIcon         = LoadIcon(hInst, "Icon");
   wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   wndclass.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
   wndclass.lpszMenuName  = "MENU";
   wndclass.lpszClassName = szAppName;

   return(RegisterClass(&wndclass));
}

//
// InitInstance -- initializes this instance of app, creates windows.
//
BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{
   hFrame = CreateWindow(  szAppName, szAppName,
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           NULL, NULL, hInstance, NULL);
   if (!hFrame)
      return(FALSE);

   SizeWindow();                             // set window & char sizes
   ShowWindow(hFrame, nCmdShow);
   UpdateWindow(hFrame);

   return(TRUE);                             // connections
}

//
// SizeWindow -- sets window's character and external dimensions.
//
void SizeWindow()
{
   HDC hdc;
   TEXTMETRIC tm;
   RECT rect;
   int ychar, xchar;

   hdc = GetDC(hFrame);
   SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
   GetTextMetrics(hdc, &tm);
   ychar = tm.tmHeight + tm.tmExternalLeading;
   xchar = tm.tmAveCharWidth;
   ReleaseDC(hFrame, hdc);

   // set initial window width & height in chars
   GetWindowRect(hFrame, &rect);
   MoveWindow( hFrame, rect.left, rect.top,
               40 * xchar,
               10 * ychar + GetSystemMetrics(SM_CYCAPTION) +
               GetSystemMetrics(SM_CYMENU), FALSE);
}
 
//
// FrameWndProc -- callback function for application frame (main) window.
// Decodes message and routes to appropriate message handler. If no handler
// found, calls DefWindowProc.
// 
LONG FAR APIENTRY FrameWndProc(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   int i;

   for (i = 0; i < dim(frameMsgs); i++)
   {
      if (wMsg == frameMsgs[i].Code)
         return(*frameMsgs[i].Fxn)(hWnd, wMsg, wParam, lParam);
   }

   return(DefWindowProc(hWnd, wMsg, wParam, lParam));
}

//
// DoCommand -- demultiplexes WM_COMMAND messages resulting from menu
// selections, and routes to corresponding menu item handler.  Sends back
// any unrecognized messages to windows.
//
LONG DoCommand(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   int i;

   for (i = 0; i < dim(menuItems); i++)
   {
      if (wParam == menuItems[i].Code)
         return(*menuItems[i].Fxn)(hWnd, wMsg, wParam, lParam);
   }

   return(DefWindowProc(hWnd, wMsg, wParam, lParam));
}

//
// DoClose -- responds to close message by refusing pending client connects
// and terminating connections in progress, then kills window.
//
LONG DoClose(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   CloseSockets();

   DestroyWindow(hWnd);
   return(FALSE);
}

//
// DoDestroy -- posts a WM_QUIT message to the task's win queue, which
// causes the main translate & dispatch loop to exit, and the app to
// terminate.
//
LONG DoDestroy(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   PostQuitMessage(0);
   return(FALSE);
}

//
// DoPaint -- Paint the client window with the contents of the
// transfer buffer.
//
LONG DoPaint(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   RECT rect;                 // client window dimensions
   HDC hdc;
   PAINTSTRUCT ps;
   struct list_of_lists *list;
   struct list_item *item;
   int counter;
   char trash[128];

   Buf[0] = 0;

   list = list_of_list_head;

   while (list)
   {
      counter = 0;
      item = list->list;

      while (item) 
      {
         counter++;
         item = item->next;
      }

      sprintf(trash, "%d in %s\n", counter, list->list_id);
      strcat(Buf, trash);
      
      list=list->next;      
   }

   hdc = BeginPaint(hWnd, &ps);
   GetClientRect(hWnd, &rect);
   DrawText(hdc, Buf, strlen(Buf), &rect, DT_EXPANDTABS);
   EndPaint(hWnd, &ps);
 
   return(FALSE);
}

//
// DoMenuAbout -- respond to "About..." menu selection by invoking the
// "About" dialog box.
//
LONG DoMenuAbout(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   WNDPROC lpProcAbout;

   lpProcAbout = MakeProcInstance((WNDPROC)AboutDlgProc, hInst);
   DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
   FreeProcInstance(lpProcAbout);

   return(FALSE);
}

//
// AboutDlgProc -- callback for the "About" dialog box
//
BOOL FAR APIENTRY AboutDlgProc(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   if ((wMsg == WM_COMMAND) && (wParam == IDOK))   // dismiss dialog if OK
      EndDialog(hWnd, 0);
 
   return(FALSE);                                  // otherwise just sit there
}

//
// UpdateDisplay -- display is xfer buffer, so we store the size, and
// force a repaint.
//
void UpdateDisplay()
{
   InvalidateRect(hFrame, NULL, TRUE);
}

//
// DoListen -- set up a listen socket to listen for client connection
// requests.  The WM_CONNECTION will signal that such is pending.
//
LONG DoListen(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   SOCKADDR_IN sin;

   ListenSocket = ConnectSocket = INVALID_SOCKET;

   if ((ListenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
   {
      DisplayWSError();
      return(FALSE);
   }

   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = INADDR_ANY;
   sin.sin_port = htons(SERVERPORT);

   if (bind(ListenSocket, (LPSOCKADDR) &sin, sizeof(sin)))
   {
      DisplayWSError();
      return(FALSE);
   }

   if (listen(ListenSocket, 5))
   {
      DisplayWSError();
      return(FALSE);
   }

   WSAAsyncSelect(ListenSocket, hFrame, WM_CONNECTION, FD_ACCEPT);
   return(FALSE);
}


//
// DoConnection -- opens a connection to requesting client, and
// signals interest in data for read.  We'll receive a WM_GETREQUEST
// message when data has arrived.
//
LONG DoConnection(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   struct sockaddr sad;   /* Adress of listening socket */
   int len = sizeof(sad);

   /* take connection request off queue, and disable further
      notifications.  we can process only one request at a time. */

   ConnectSocket = accept(ListenSocket, &sad, &len);
   WSAAsyncSelect(ListenSocket, hFrame, 0, 0);

   if (ConnectSocket == INVALID_SOCKET)
   { 
      DisplayWSError();
      return(FALSE);
   }

   // notify us when a request (or part of one) has arrived
   WSAAsyncSelect(ConnectSocket, hFrame, WM_GETREQUEST, FD_READ);

   return(FALSE);
}

//
// CloseSockets -- shuts down listen, and any transfer in progress.
//
void CloseSockets(void)
{
   if (ListenSocket != INVALID_SOCKET);
      closesocket(ListenSocket);
   if (ConnectSocket != INVALID_SOCKET);
      closesocket(ConnectSocket);
}

//
// DoGetRequest -- synchronously receives the client's query, and places
// it in the request buffer.  We expect <CR><LF> terminator, but will accept
// end-of-stream (zero recv return).  We then ask for a write-space-available
// notification.
//
LONG DoGetRequest(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   char *p;
   int nchars;

   Request[0] = ReqLen = 0;
   Blocking(ConnectSocket);               // going synchronous now

   do // until we've received the whole request
   {
      nchars = recv(ConnectSocket, (LPSTR) Buf, sizeof(Buf), 0);

      if (nchars == SOCKET_ERROR)
      {
         DisplayWSError();
         return(FALSE);
      }

      Buf[nchars] = 0;                    // null terminate the buffer

      if ((ReqLen += nchars) > REQLEN)    // prevent request overflow
      {
         SendError("query too long");     // tell client
         return(FALSE);                   // request overflow, bail
      }

      strcat(Request, Buf);               // append to request
      p = strstr(Request, "\r\n");        // end-of-request?

   }
   while (nchars && !p);                  // stop for either end-of-stream
                                          // or <CR><LF> terminator
   if (p) *p = 0;                         // chop the terminator
   
   if (!Request[0])                       // translate null request to default
      strcpy(Request, DEFAULTFILE);

   // going asychronous now, this is will notify us of when to reply
   WSAAsyncSelect(ConnectSocket, hFrame, WM_REPLY, FD_WRITE);

   return(FALSE);
}

/** ZAPS CODE BEGINS HERE ****/


/* Find an item in the list-of-lists, create one if not there already! */

struct list_of_lists *FindLOLItem(char *listID)
{
   struct list_of_lists **temp;

   temp = &list_of_list_head;

   while (*temp) {
      if (strcmp((*temp)->list_id, listID) == 0) return *temp;
      temp = &(*temp)->next;
   }

   *temp = calloc(sizeof(struct list_of_lists),1);

   if (!*temp) return NULL;

   (*temp)->list_id = _strdup(listID);
   (*temp)->next    = NULL;
   (*temp)->list    = NULL;

   return (*temp);   
}

void AddToList(char *listID, struct in_addr *addr)
{
   struct list_of_lists *list;
   struct list_item **temp;

   list = FindLOLItem(listID);

   /* Should never happen, but... */
   if (NULL == list) 
      return;

   temp = &list->list;

   while (*temp)
   {
      if (memcmp(&(*temp)->ip, addr, sizeof(struct in_addr)) == 0) return;
      temp = &(*temp)->next;
   }

   *temp = calloc(sizeof(struct list_item),1);

   if (!*temp) return;

   memcpy(&(*temp)->ip, addr, sizeof(struct in_addr));
   (*temp)->next    = NULL;
}

void SendList(char *ListID)
{
   struct list_of_lists *list = NULL;
   struct list_item *stepper = NULL;
   int nchars;
   
   list = FindLOLItem(ListID);
   stepper = list->list;


   while (stepper)
   {
      sprintf(Buf, "%s\n", inet_ntoa(stepper->ip));
      nchars = strlen(Buf);

      if (send(ConnectSocket, Buf, nchars, 0) == SOCKET_ERROR)
      {
         DisplayWSError();
         return;
      }
      stepper = stepper->next;
   }
}

/* Send the first item in the list that is nonequal to the calling host */
void SendOne(char *ListID, struct in_addr *addr)
{
   struct list_of_lists *list = NULL;
   struct list_item *stepper = NULL;
   int nchars;
   
   list = FindLOLItem(ListID);
   stepper = list->list;

   while (stepper)
   {
      /* Compare with senders IP */
      if (memcmp(addr, &stepper->ip, sizeof(struct in_addr)))
      {
         sprintf(Buf, "%s\n", inet_ntoa(stepper->ip));
         nchars = strlen(Buf);

         if (send(ConnectSocket, Buf, nchars, 0) == SOCKET_ERROR)
         {
            DisplayWSError();
         }
         
         return;
      }
      stepper = stepper->next;
   }

   /* Nobody is there!! */

   sprintf(Buf, "-\n");
   nchars = strlen(Buf);

   if (send(ConnectSocket, Buf, nchars, 0) == SOCKET_ERROR)
   {
      DisplayWSError();
      return;
   }
}


void DeleteFromList(char *ListID, struct in_addr *addr)
{
   struct list_of_lists *list = NULL;
   struct list_item **stepper = NULL;
   
   list = FindLOLItem(ListID);
   stepper = &list->list;

   while (*stepper)
   {
      if (0 == memcmp(&(*stepper)->ip, addr, sizeof(struct in_addr)))
      {
         struct list_item *temp;

         /* Remember the old one... */
         temp = (*stepper);

         /* relink it to next */
         (*stepper) = (*stepper)->next;

         /* Free the old one */         
         free(temp);
         return;
      }

      stepper = &(*stepper)->next;
   }
}

//
// DoReply -- synchronously replies to client with either the requested
// file or an error text, then reenables accept notifies, so we can
// process the next request.
//
LONG DoReply(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
{
   struct sockaddr_in this;
   int len = sizeof(this);
   char *p;

   Blocking(ConnectSocket);               // going synchronous now

   /* Get the peer's name */
   getpeername(ConnectSocket, &this, &len);

   switch (Request[0])
   {
      case 'A':
         AddToList(Request+1, &this.sin_addr);
         SendOne(Request+1, &this.sin_addr);
         UpdateDisplay();
         break;
      case 'D':
         DeleteFromList(Request+1, &this.sin_addr);
         SendString("OK");
         UpdateDisplay();
         break;
      case '?':
         SendList(Request+1);
         break;

      /* An IP number is sent */
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
         if (p = strchr(Request, ':'))
         {
            *p++ = 0;
            this.sin_addr.S_un.S_addr = inet_addr(Request);
            DeleteFromList(p, &this.sin_addr);
            SendString("OK");
            UpdateDisplay();
         }
         else
            SendError("Invalid request");
         break;

      default:
         SendError("Invalid request");
         break;
   }

   closesocket(ConnectSocket);
   ConnectSocket = INVALID_SOCKET;

   // we're ready for the next request now
   WSAAsyncSelect(ListenSocket, hFrame, WM_CONNECTION, FD_ACCEPT);

   return(FALSE);
}

/***** ZAPS CODE ENDS HERE ****/

//
// Blocking -- sets socket to block.
//
void Blocking(SOCKET s)
{
   u_long nonblock = FALSE;               // yes, we have no bananas

   WSAAsyncSelect(s, hFrame, 0, 0);       // turn off message notifications
   ioctlsocket(s, FIONBIO, &nonblock);    // set socket to blocking
}

   
//
// SendError -- sends an error string to client.
//
void SendError(char *errstr)
{
   static char *prefix = "Error: ";

   send(ConnectSocket, prefix, strlen(prefix), 0);
   send(ConnectSocket, errstr, strlen(errstr), 0);

   closesocket(ConnectSocket);
   ConnectSocket = INVALID_SOCKET;
}

//
// SendString -- sends a text string to client.
//
void SendString(char *str)
{
   send(ConnectSocket, str, strlen(str), 0);

   closesocket(ConnectSocket);
   ConnectSocket = INVALID_SOCKET;
}

//
// DisplayWSError -- displays the winsock error in the client window.
//
void DisplayWSError(void)
{
   strcpy(Buf, WSErrorString(WSAGetLastError()));
   UpdateDisplay(strlen(Buf));

   closesocket(ConnectSocket);
   ConnectSocket = INVALID_SOCKET;
}

//
// WSErrorString -- translates winsock error to appropriate string.
//
char *WSErrorString(UINT err)
{
   int i;
   static char szerr[80];

   for (i = 0; i < dim(WSErrors); i++)
      if (err == WSErrors[i].err)
         return(WSErrors[i].sztext);

   sprintf(szerr, "Windows Sockets reports error %04x", err);
   return(szerr);
}

//
// DebugOut -- for outputting debug info to the AUX device (or debugger).
//
void DebugOut(char FAR *lps, ...)
{
   static char buf[80];
   static char FAR *args;

   args = (char FAR *) &lps + sizeof(lps);
   wvsprintf(buf, lps, args);
   OutputDebugString(buf);
}
