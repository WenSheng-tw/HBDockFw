#include "hbapi.h"
#include "hbapiitm.h"

#include "hbdockhb.h"
#include "hbdockpanel.h"
#include "hbdockpanelname.h"
#include "hbdockpaneltext.h"
#include "hbdockpanelparent.h"
#include "hbdockpanelfloating.h"
#include "hbdockpaneldock.h"

/*
 * pPanel := HBDOCKPANELNEW()
 *
 * Crea un panel de docking vacio, sin ventana de contenido
 * todavia (usar HBDOCKPANELSETHWND() para asociarle una).
 */
HB_FUNC( HBDOCKPANELNEW )
{
   hb_dockRetHandle(
      ( void * ) hbDockPanelNew() );
}

/*
 * HBDOCKPANELDELETE( pPanel )
 */
HB_FUNC( HBDOCKPANELDELETE )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   hbDockPanelDelete( pPanel );
}

/*
 * HBDOCKPANELSETNAME( pPanel, cName )
 *
 * Identificador interno unico del panel (para buscarlo
 * despues con HBDOCKMANAGERFINDPANEL()). No es lo que
 * ve el usuario: para eso esta el Caption.
 */
HB_FUNC( HBDOCKPANELSETNAME )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   hbDockPanelSetName(
      pPanel,
      hb_parc( 2 ) );
}

/* cName := HBDOCKPANELGETNAME( pPanel ) */
HB_FUNC( HBDOCKPANELGETNAME )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   hb_retc( hbDockPanelGetName( pPanel ) );
}

/*
 * HBDOCKPANELSETCAPTION( pPanel, cCaption )
 *
 * Texto que se dibuja en la barra de titulo del panel.
 */
HB_FUNC( HBDOCKPANELSETCAPTION )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   hbDockPanelSetCaption(
      pPanel,
      hb_parc( 2 ) );
}

/* cCaption := HBDOCKPANELGETCAPTION( pPanel ) */
HB_FUNC( HBDOCKPANELGETCAPTION )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   hb_retc( hbDockPanelGetCaption( pPanel ) );
}

/*
 * HBDOCKPANELSETHWND( pPanel, hWnd )
 *
 * hWnd es la ventana real de contenido del panel (por
 * ejemplo, ::hWnd de un TGet, TBrowse, o cualquier control
 * FiveWin ya creado). HBDock32 la reparenta/redimensiona
 * segun donde este acoplado o flotando el panel.
 */
HB_FUNC( HBDOCKPANELSETHWND )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   HWND hWnd = ( HWND ) hb_dockParamHandle( 2 );

   if( pPanel != NULL )
      pPanel->hWnd = hWnd;
}

/* hWnd := HBDOCKPANELGETHWND( pPanel ) */
HB_FUNC( HBDOCKPANELGETHWND )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   hb_dockRetHandle(
      pPanel != NULL ? ( void * ) pPanel->hWnd : NULL );
}

/*
 * HBDOCKPANELSETMINSIZE( pPanel, nWidth, nHeight )
 */
HB_FUNC( HBDOCKPANELSETMINSIZE )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   if( pPanel != NULL )
   {
      pPanel->MinWidth  = hb_parni( 2 );
      pPanel->MinHeight = hb_parni( 3 );
   }
}

/*
 * HBDOCKPANELSETPREFERREDSIZE( pPanel, nSize )
 *
 * Ancho (si el panel se acopla a LEFT/RIGHT) o alto (si se
 * acopla a TOP/BOTTOM) que va a tener al acoplarse o
 * flotar por primera vez.
 */
HB_FUNC( HBDOCKPANELSETPREFERREDSIZE )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   if( pPanel != NULL )
      pPanel->PreferredSize = hb_parni( 2 );
}

/* lFloating := HBDOCKPANELISFLOATING( pPanel ) */
HB_FUNC( HBDOCKPANELISFLOATING )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   hb_retl( hbDockPanelIsFloating( pPanel ) );
}

/* nSite := HBDOCKPANELGETDOCKSITE( pPanel )
 * (ver hbdock.ch para las constantes HBDOCK_LEFT/...) */
HB_FUNC( HBDOCKPANELGETDOCKSITE )
{
   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 1 );

   hb_retni( ( int ) hbDockPanelGetDockSite( pPanel ) );
}
