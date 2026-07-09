#include "hbapi.h"
#include "hbapiitm.h"

#include "hbdockhb.h"
#include "hbdockmanager.h"
#include "hbdockmanagerdock.h"
#include "hbdockmanagerfloat.h"
#include "hbdockmanagerautohide.h"

/*
 * pPanel := HBDOCKMANAGERFINDPANEL( pManager, cName )
 *
 * Busca un panel ya registrado por su Name (ver
 * HBDOCKPANELSETNAME()). Devuelve NIL si no existe.
 */
HB_FUNC( HBDOCKMANAGERFINDPANEL )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel = hbDockManagerFindPanel(
      pManager,
      hb_parc( 2 ) );

   if( pPanel != NULL )
      hb_dockRetHandle( ( void * ) pPanel );
}

/*
 * HBDOCKMANAGERADDPANEL( pManager, pPanel )
 *
 * Registra el panel en el manager (lista plana, usada para
 * iterarlos y para el ciclo de vida). No lo acopla todavia:
 * para eso ver HBDOCKMANAGERDOCKPANEL().
 */
HB_FUNC( HBDOCKMANAGERADDPANEL )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 2 );

   hbDockManagerAddPanel(
      pManager,
      pPanel );
}

/*
 * HBDOCKMANAGERREMOVEPANEL( pManager, pPanel )
 *
 * Lo saca de la lista del manager (y del arbol, si estaba
 * acoplado). No libera el panel: eso es HBDOCKPANELDELETE().
 */
HB_FUNC( HBDOCKMANAGERREMOVEPANEL )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 2 );

   hbDockManagerUndockPanel(
      pManager,
      pPanel );

   hbDockManagerRemovePanel(
      pManager,
      pPanel );
}

/*
 * HBDOCKMANAGERDOCKPANEL( pManager, pPanel, nSite )
 *
 * Acopla el panel contra el borde nSite (HBDOCK_LEFT,
 * HBDOCK_RIGHT, HBDOCK_TOP, HBDOCK_BOTTOM o HBDOCK_CENTER,
 * ver hbdock.ch) de lo que ya este acoplado. El primer
 * panel que acoples se convierte en el unico contenido
 * del arbol sea cual sea nSite.
 */
HB_FUNC( HBDOCKMANAGERDOCKPANEL )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 2 );

   HB_DOCK_SITE Site =
      ( HB_DOCK_SITE ) hb_parni( 3 );

   hbDockManagerDockPanelAt(
      pManager,
      pManager != NULL ? pManager->LayoutEngine.Root : NULL,
      pPanel,
      Site );

   hbDockManagerLayout( pManager );
}

/*
 * HBDOCKMANAGERUNDOCKPANEL( pManager, pPanel )
 *
 * Saca el panel del arbol (queda oculto, sin flotar ni
 * autohide, hasta que lo vuelvas a acoplar/flotar).
 */
HB_FUNC( HBDOCKMANAGERUNDOCKPANEL )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 2 );

   hbDockManagerUndockPanel(
      pManager,
      pPanel );

   hbDockManagerLayout( pManager );
}

/*
 * HBDOCKMANAGERFLOATPANEL( pManager, pPanel, nX, nY )
 *
 * Convierte el panel (acoplado o no) en una ventana
 * flotante independiente, ubicada en (nX, nY) en
 * coordenadas de pantalla. Requiere haber llamado antes
 * HBDOCKMANAGERSETINSTANCE().
 */
HB_FUNC( HBDOCKMANAGERFLOATPANEL )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 2 );

   POINT pt;

   pt.x = hb_parni( 3 );
   pt.y = hb_parni( 4 );

   hbDockManagerFloatPanel(
      pManager,
      pPanel,
      pt );
}

/*
 * HBDOCKMANAGERDOCKBACK( pManager, pPanel, nSite )
 *
 * Vuelve a acoplar un panel flotante contra el borde nSite
 * de la raiz del arbol, destruyendo su ventana flotante.
 */
HB_FUNC( HBDOCKMANAGERDOCKBACK )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 2 );

   HB_DOCK_SITE Site =
      ( HB_DOCK_SITE ) hb_parni( 3 );

   hbDockManagerDockPanel(
      pManager,
      pPanel,
      pManager != NULL ? pManager->LayoutEngine.Root : NULL,
      Site );
}

/*
 * HBDOCKMANAGERAUTOHIDEPANEL( pManager, pPanel )
 *
 * Repliega el panel acoplado a una pestana fina en su
 * borde actual.
 */
HB_FUNC( HBDOCKMANAGERAUTOHIDEPANEL )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 2 );

   hbDockManagerAutoHidePanel(
      pManager,
      pPanel );
}

/*
 * HBDOCKMANAGERAUTOHIDERESTORE( pManager, pPanel )
 *
 * Vuelve a acoplar en el arbol un panel que estaba
 * replegado en autohide.
 */
HB_FUNC( HBDOCKMANAGERAUTOHIDERESTORE )
{
   HB_DOCK_MANAGER * pManager =
      ( HB_DOCK_MANAGER * ) hb_parptr( 1 );

   HB_DOCK_PANEL * pPanel =
      ( HB_DOCK_PANEL * ) hb_parptr( 2 );

   hbDockManagerAutoHideRestore(
      pManager,
      pPanel );
}
