#include <windows.h>
#include <windowsx.h>

#include "hbdockhost.h"
#include "hbdocklayout.h"
#include "hbdockhittest.h"
#include "hbdocktreesplitfind.h"
#include "hbdockcaption.h"
#include "hbdockmouse.h"
#include "hbdockmanagerdrag.h"
#include "hbdockmanagerautohide.h"
#include "hbdockmanagerlayout.h"
#include "hbdocksplitter.h"
#include "hbdockguides.h"
#include "hbdockpreview.h"
#include "hbdockpaneltext.h"

#define HBDOCK_DRAG_THRESHOLD_CX   4
#define HBDOCK_DRAG_THRESHOLD_CY   4

BOOL hbDockHostAttach(
   HB_DOCK_HOST * pHost,
   HWND hWnd,
   HB_DOCK_MANAGER * pManager )
{
   if( pHost == NULL )
      return FALSE;

   ZeroMemory(
      pHost,
      sizeof( HB_DOCK_HOST ) );

   pHost->hWnd = hWnd;
   pHost->pManager = pManager;

   if( pManager != NULL )
      pManager->hParent = hWnd;

   return TRUE;
}

void hbDockHostDetach(
   HB_DOCK_HOST * pHost )
{
   if( pHost == NULL )
      return;

   if( pHost->hWnd != NULL )
      KillTimer(
         pHost->hWnd,
         HBDOCK_AUTOHIDE_TIMER_ID );

   pHost->hWnd = NULL;
   pHost->pManager = NULL;
}

void hbDockHostResize(
   HB_DOCK_HOST * pHost )
{
   if( pHost == NULL )
      return;

   if( pHost->pManager == NULL )
      return;

   hbDockPerformLayout(
      pHost->pManager );
}

void hbDockHostInvalidate(
   HB_DOCK_HOST * pHost )
{
   if( pHost == NULL )
      return;

   if( pHost->hWnd != NULL )
      InvalidateRect(
         pHost->hWnd,
         NULL,
         TRUE );
}

/* Encuentra, si lo hay, el nodo hoja bajo pt cuyo caption
 * (franja superior del panel) contiene el punto. */
static HB_DOCK_NODE * hbDockHostFindCaptionAt(
   HB_DOCK_MANAGER * pManager,
   POINT pt )
{
   HB_DOCK_HITTEST Hit;
   RECT rc;

   hbDockHitTestTree(
      pManager->LayoutEngine.Root,
      pt,
      &Hit );

   if( Hit.Hit != HB_DOCK_HIT_PANEL || Hit.Node == NULL )
      return NULL;

   rc = Hit.Node->Rect;
   rc.bottom = rc.top + HBDOCK_CAPTION_HEIGHT;

   if( !PtInRect( &rc, pt ) )
      return NULL;

   return Hit.Node;
}

static void hbDockHostUpdateAutoHideHover(
   HB_DOCK_HOST * pHost,
   POINT pt )
{
   HB_DOCK_PANEL * pHit;

   pHit = hbDockManagerAutoHideHitTest(
      pHost->pManager,
      pt );

   if( pHit != NULL && pHit != pHost->HoverAutoHide )
   {
      if( pHost->HoverAutoHide != NULL )
         hbDockManagerAutoHideCollapse(
            pHost->pManager,
            pHost->HoverAutoHide );

      pHost->HoverAutoHide = pHit;

      hbDockManagerAutoHideExpand(
         pHost->pManager,
         pHost->HoverAutoHide );

      SetTimer(
         pHost->hWnd,
         HBDOCK_AUTOHIDE_TIMER_ID,
         HBDOCK_AUTOHIDE_TIMER_MS,
         NULL );

      hbDockHostInvalidate( pHost );
   }
}

static void hbDockHostCheckAutoHideLeave(
   HB_DOCK_HOST * pHost )
{
   POINT pt;
   RECT rc;

   if( pHost->HoverAutoHide == NULL )
      return;

   GetCursorPos( &pt );
   ScreenToClient( pHost->hWnd, &pt );

   /* El panel expandido ocupa su Rect actual mientras
    * esta desplegado (ver hbDockAutoHideExpand). */
   rc = pHost->HoverAutoHide->Rect;

   if( !PtInRect( &rc, pt ) &&
       hbDockManagerAutoHideHitTest(
          pHost->pManager, pt ) != pHost->HoverAutoHide )
   {
      hbDockManagerAutoHideCollapse(
         pHost->pManager,
         pHost->HoverAutoHide );

      pHost->HoverAutoHide = NULL;

      KillTimer(
         pHost->hWnd,
         HBDOCK_AUTOHIDE_TIMER_ID );

      hbDockHostInvalidate( pHost );
   }
}

static void hbDockHostPaintNode(
   HDC hDC,
   HB_DOCK_NODE * pNode )
{
   if( pNode == NULL )
      return;

   if( pNode->Type == HB_DOCK_NODE_LEAF )
   {
      if( pNode->Panel != NULL && pNode->Panel->Visible )
      {
         HB_DOCK_CAPTION Caption;
         RECT rc;

         rc = pNode->Rect;
         rc.bottom = rc.top + HBDOCK_CAPTION_HEIGHT;

         hbDockCaptionInit(
            &Caption,
            pNode->Panel );

         hbDockCaptionLayout(
            &Caption,
            &rc );

         hbDockCaptionDraw(
            hDC,
            &Caption );
      }

      return;
   }

   {
      HB_DOCK_SPLITTER Splitter;
      RECT rc = pNode->Rect;

      hbDockSplitterInit(
         &Splitter );

      if( pNode->Type == HB_DOCK_NODE_SPLIT_H )
      {
         rc.left  = pNode->Rect.left + pNode->SplitPos;
         rc.right = rc.left + pNode->SplitterSize;
      }
      else
      {
         rc.top    = pNode->Rect.top + pNode->SplitPos;
         rc.bottom = rc.top + pNode->SplitterSize;
      }

      Splitter.Vertical =
         ( pNode->Type == HB_DOCK_NODE_SPLIT_H );

      hbDockSplitterSetRect(
         &Splitter,
         &rc );

      Splitter.Visible = 1;

      hbDockSplitterDraw(
         hDC,
         &Splitter );
   }

   hbDockHostPaintNode(
      hDC,
      pNode->First );

   hbDockHostPaintNode(
      hDC,
      pNode->Second );
}

static void hbDockHostPaintAutoHide(
   HDC hDC,
   HB_DOCK_MANAGER * pManager )
{
   int i;
   HBRUSH hBrush;

   for( i = 0; i < pManager->AutoHidePanels.Count; ++i )
   {
      HB_DOCK_AUTOHIDE * pAutoHide =
         ( HB_DOCK_AUTOHIDE * ) hbDockArrayGet(
            &pManager->AutoHidePanels,
            i );

      if( pAutoHide == NULL )
         continue;

      hBrush = CreateSolidBrush(
         GetSysColor( COLOR_BTNFACE ) );

      FillRect(
         hDC,
         &pAutoHide->HiddenRect,
         hBrush );

      DeleteObject( hBrush );

      DrawEdge(
         hDC,
         &pAutoHide->HiddenRect,
         EDGE_RAISED,
         BF_RECT );

      if( pAutoHide->Panel != NULL )
      {
         SetBkMode( hDC, TRANSPARENT );

         DrawText(
            hDC,
            hbDockPanelGetCaption( pAutoHide->Panel ),
            -1,
            &pAutoHide->HiddenRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE );
      }
   }
}

static void hbDockHostPaintDragOverlay(
   HDC hDC,
   HB_DOCK_MANAGER * pManager )
{
   int i;

   if( !pManager->DragTracker.Context.Dragging )
      return;

   for( i = 0; i < pManager->DragTracker.Guides.Count; ++i )
   {
      HB_DOCK_DOCKSITE * pSite =
         &pManager->DragTracker.Guides.Guide[ i ];

      HBRUSH hBrush = CreateSolidBrush(
         pSite->Site == pManager->DragTracker.Context.CurrentSite ?
            RGB( 0, 120, 215 ) : RGB( 200, 200, 200 ) );

      FrameRect(
         hDC,
         &pSite->Rect,
         hBrush );

      DeleteObject( hBrush );
   }

   hbDockPreviewDraw(
      hDC,
      &pManager->DragTracker.Preview );
}

void hbDockHostPaint(
   HB_DOCK_HOST * pHost,
   HDC hDC )
{
   if( pHost == NULL || pHost->pManager == NULL || hDC == NULL )
      return;

   hbDockHostPaintNode(
      hDC,
      pHost->pManager->LayoutEngine.Root );

   hbDockHostPaintAutoHide(
      hDC,
      pHost->pManager );

   hbDockHostPaintDragOverlay(
      hDC,
      pHost->pManager );
}

BOOL hbDockHostHandleMessage(
   HB_DOCK_HOST * pHost,
   UINT uMsg,
   WPARAM wParam,
   LPARAM lParam )
{
   POINT pt;

   if( pHost == NULL || pHost->pManager == NULL )
      return FALSE;

   switch( uMsg )
   {
      case WM_LBUTTONDOWN:
      {
         HB_DOCK_NODE * pSplitNode;
         HB_DOCK_NODE * pCaptionNode;

         pt.x = GET_X_LPARAM( lParam );
         pt.y = GET_Y_LPARAM( lParam );

         pSplitNode = hbDockTreeFindSplitterAt(
            pHost->pManager->LayoutEngine.Root,
            pt );

         if( pSplitNode != NULL )
         {
            hbDockSplitNodeInit(
               &pHost->ActiveSplit,
               pSplitNode );

            hbDockSplitNodeUpdate(
               &pHost->ActiveSplit );

            hbDockSplitterTrackerBegin(
               &pHost->SplitterTracker,
               &pHost->ActiveSplit,
               pt );

            SetCapture( pHost->hWnd );

            return TRUE;
         }

         pCaptionNode = hbDockHostFindCaptionAt(
            pHost->pManager,
            pt );

         if( pCaptionNode != NULL && pCaptionNode->Panel != NULL )
         {
            pHost->PendingDragPanel = pCaptionNode->Panel;
            pHost->PendingDragPoint = pt;

            SetCapture( pHost->hWnd );

            return TRUE;
         }

         return FALSE;
      }

      case WM_MOUSEMOVE:

         pt.x = GET_X_LPARAM( lParam );
         pt.y = GET_Y_LPARAM( lParam );

         if( pHost->SplitterTracker.Tracking )
         {
            hbDockSplitterTrackerMove(
               &pHost->SplitterTracker,
               pt );

            hbDockManagerLayout(
               pHost->pManager );

            hbDockHostInvalidate( pHost );

            return TRUE;
         }

         if( pHost->Dragging )
         {
            hbDockManagerDragMove(
               pHost->pManager,
               pt );

            hbDockHostInvalidate( pHost );

            return TRUE;
         }

         if( pHost->PendingDragPanel != NULL )
         {
            if( hbDockMouseMoved(
                   pHost->PendingDragPoint,
                   pt,
                   HBDOCK_DRAG_THRESHOLD_CX,
                   HBDOCK_DRAG_THRESHOLD_CY ) )
            {
               hbDockManagerBeginDrag(
                  pHost->pManager,
                  pHost->PendingDragPanel,
                  pHost->PendingDragPoint );

               pHost->PendingDragPanel = NULL;
               pHost->Dragging = 1;

               hbDockManagerDragMove(
                  pHost->pManager,
                  pt );

               hbDockHostInvalidate( pHost );
            }

            return TRUE;
         }

         hbDockHostUpdateAutoHideHover(
            pHost,
            pt );

         return FALSE;

      case WM_LBUTTONUP:

         pt.x = GET_X_LPARAM( lParam );
         pt.y = GET_Y_LPARAM( lParam );

         if( pHost->SplitterTracker.Tracking )
         {
            hbDockSplitterTrackerEnd(
               &pHost->SplitterTracker );

            ReleaseCapture();

            return TRUE;
         }

         if( pHost->Dragging )
         {
            hbDockManagerEndDrag(
               pHost->pManager,
               pt );

            pHost->Dragging = 0;

            ReleaseCapture();

            hbDockHostInvalidate( pHost );

            return TRUE;
         }

         if( pHost->PendingDragPanel != NULL )
         {
            /* Fue un click simple, sin arrastre: solo
             * activa el panel bajo el caption. */
            pHost->PendingDragPanel = NULL;

            ReleaseCapture();

            return TRUE;
         }

         return FALSE;

      case WM_CAPTURECHANGED:

         /* Otro control se robo la captura del mouse:
          * cancela cualquier operacion en curso en vez
          * de dejarla en un estado inconsistente. */
         if( pHost->Dragging )
         {
            hbDockManagerCancelDrag(
               pHost->pManager );

            pHost->Dragging = 0;
         }

         if( pHost->SplitterTracker.Tracking )
            hbDockSplitterTrackerEnd(
               &pHost->SplitterTracker );

         pHost->PendingDragPanel = NULL;

         return FALSE;

      case WM_TIMER:

         if( wParam == HBDOCK_AUTOHIDE_TIMER_ID )
         {
            hbDockHostCheckAutoHideLeave(
               pHost );

            return TRUE;
         }

         return FALSE;
   }

   return FALSE;
}
