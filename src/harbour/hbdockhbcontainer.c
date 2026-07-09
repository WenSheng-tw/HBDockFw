#include "hbapi.h"
#include "hbapiitm.h"

#include "hbdockhb.h"
#include "hbdockcontainer.h"

static int s_bClassRegistered = 0;

/*
 * hWnd := HBDOCKCONTAINERCREATE( hWndParent, hInstance, pManager )
 *
 * Crea la ventana hija que aloja visualmente todo el
 * docking (paneles, splitters, autohide, drag&drop) y ya
 * queda completamente funcional: no hace falta subclasear
 * nada mas. hWndParent es normalmente el area cliente MDI
 * o la ventana principal.
 */
HB_FUNC( HBDOCKCONTAINERCREATE )
{
   HWND hWndParent =
      ( HWND ) hb_dockParamHandle( 1 );

   HINSTANCE hInstance =
      ( HINSTANCE ) hb_dockParamHandle( 2 );

   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 3 );

   if( !s_bClassRegistered )
   {
      hbDockContainerRegisterClass( hInstance );
      s_bClassRegistered = 1;
   }

   hb_dockRetHandle(
      ( void * ) hbDockContainerCreate(
         hWndParent,
         hInstance,
         pManager ) );
}
