#include <windows.h>
#include <stdlib.h>

#include "hbdockcontainer.h"
#include "hbdocklayout.h"
#include "hbdockhost.h"

#define HB_DOCK_CONTAINER_CLASS "HBDockContainer"

static LRESULT CALLBACK hbDockContainerProc(
   HWND hWnd,
   UINT msg,
   WPARAM wParam,
   LPARAM lParam )
{
   HB_DOCK_HOST * pHost;

   pHost = ( HB_DOCK_HOST * )
      GetWindowLongPtr(
         hWnd,
         GWLP_USERDATA );

   switch( msg )
   {
      case WM_CREATE:
      {
         CREATESTRUCT * cs;
         HB_DOCK_MANAGER * pManager;

         cs = ( CREATESTRUCT * ) lParam;
         pManager = ( HB_DOCK_MANAGER * ) cs->lpCreateParams;

         pHost = ( HB_DOCK_HOST * ) calloc(
            1,
            sizeof( HB_DOCK_HOST ) );

         if( pHost != NULL )
            hbDockHostAttach(
               pHost,
               hWnd,
               pManager );

         SetWindowLongPtr(
            hWnd,
            GWLP_USERDATA,
            ( LONG_PTR ) pHost );

         return 0;
      }

      case WM_SIZE:

         if( pHost != NULL )
            hbDockHostResize( pHost );

         return 0;

      case WM_ERASEBKGND:
         return 1;

      case WM_PAINT:
      {
         PAINTSTRUCT ps;

         BeginPaint(
            hWnd,
            &ps );

         if( pHost != NULL )
            hbDockHostPaint(
               pHost,
               ps.hdc );

         EndPaint(
            hWnd,
            &ps );

         return 0;
      }

      case WM_LBUTTONDOWN:
      case WM_MOUSEMOVE:
      case WM_LBUTTONUP:
      case WM_CAPTURECHANGED:
      case WM_TIMER:

         if( pHost != NULL &&
             hbDockHostHandleMessage(
                pHost,
                msg,
                wParam,
                lParam ) )
            return 0;

         break;

      case WM_DESTROY:

         if( pHost != NULL )
         {
            hbDockHostDetach( pHost );

            free( pHost );

            SetWindowLongPtr(
               hWnd,
               GWLP_USERDATA,
               ( LONG_PTR ) NULL );
         }

         return 0;
   }

   return DefWindowProc(
      hWnd,
      msg,
      wParam,
      lParam );
}

BOOL hbDockContainerRegisterClass(
   HINSTANCE hInstance )
{
   WNDCLASS wc;

   ZeroMemory(
      &wc,
      sizeof( wc ) );

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = hbDockContainerProc;
   wc.hInstance     = hInstance;
   wc.hCursor       = LoadCursor(
                         NULL,
                         IDC_ARROW );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_BTNFACE + 1 );
   wc.lpszClassName = HB_DOCK_CONTAINER_CLASS;

   return RegisterClass( &wc ) != 0;
}

HWND hbDockContainerCreate(
   HWND hParent,
   HINSTANCE hInstance,
   HB_DOCK_MANAGER *pManager )
{
   return CreateWindowEx(
      0,
      HB_DOCK_CONTAINER_CLASS,
      "",
      WS_CHILD |
      WS_VISIBLE |
      WS_CLIPSIBLINGS |
      WS_CLIPCHILDREN,
      0,
      0,
      0,
      0,
      hParent,
      NULL,
      hInstance,
      pManager );
}
