#include "hbapi.h"
#include "hbapiitm.h"

#include "hbdockhb.h"
#include "hbdockmanager.h"
#include "hbdockmanagerlayout.h"

/*
 * pManager := HBDOCKMANAGERNEW( hWndParent )
 *
 * Crea un manager de docking asociado a la ventana
 * hWndParent (tipicamente el area cliente de tu MDI, o
 * cualquier ventana que vaya a alojar el arbol de paneles).
 */
HB_FUNC( HBDOCKMANAGERNEW )
{
   HWND hWnd = ( HWND ) hb_dockParamHandle( 1 );

   hb_dockRetHandle(
      ( void * ) hbDockManagerNew( hWnd ) );
}

/*
 * HBDOCKMANAGERDELETE( pManager )
 *
 * Libera el manager y todo lo que administra (arbol de
 * paneles, ventanas flotantes vivas, paneles en autohide).
 * No libera los HB_DOCK_PANEL registrados en el: eso lo
 * hace quien los creo con HBDOCKPANELDELETE().
 */
HB_FUNC( HBDOCKMANAGERDELETE )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   hbDockManagerDelete( pManager );
}

/*
 * HBDOCKMANAGERSETINSTANCE( pManager, hInstance )
 *
 * Necesario antes de poder flotar paneles: la ventana
 * flotante se crea con esta instancia.
 */
HB_FUNC( HBDOCKMANAGERSETINSTANCE )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HINSTANCE hInstance =
      ( HINSTANCE ) hb_dockParamHandle( 2 );

   hbDockManagerSetInstance(
      pManager,
      hInstance );
}

/*
 * HBDOCKMANAGERRESIZE( pManager, nWidth, nHeight )
 *
 * Llamar desde tu handler de WM_SIZE cuando el manager NO
 * este alojado dentro de un HBDOCKCONTAINERCREATE() (que
 * ya se redimensiona solo).
 */
HB_FUNC( HBDOCKMANAGERRESIZE )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   hbDockManagerResize(
      pManager,
      hb_parni( 2 ),
      hb_parni( 3 ) );
}

/*
 * HBDOCKMANAGERLAYOUT( pManager )
 *
 * Fuerza un recalculo de layout (por ej. despues de acoplar
 * o quitar paneles a mano).
 */
HB_FUNC( HBDOCKMANAGERLAYOUT )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   hbDockManagerLayout( pManager );
}
