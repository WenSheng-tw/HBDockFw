/*
 * ---------------------------------------------------------------------------
 * HBDock32 - TDockManager
 *
 * Envoltorio OOP (estilo FiveWin) sobre HB_DOCK_MANAGER.
 * Crea ademas la ventana contenedora (HBDOCKCONTAINERCREATE)
 * que ya trae resuelto el mouse (drag&drop, splitters,
 * autohide) y el pintado del chrome de docking: no hace
 * falta subclasear nada del lado FiveWin.
 *
 * Uso tipico:
 *
 *    oDock := TDockManager():New( oWndMain, GetInstance() )
 *
 *    oPanel1 := TDockPanel():New( "PROJECT", "Project Explorer" )
 *    oPanel1:SetWindow( oBrowseProject )
 *    oPanel1:SetPreferredSize( 220 )
 *    oDock:AddPanel( oPanel1 )
 *    oDock:Dock( oPanel1, HBDOCK_LEFT )
 *
 *    oPanel2 := TDockPanel():New( "OUTPUT", "Output" )
 *    oPanel2:SetWindow( oGetOutput )
 *    oPanel2:SetPreferredSize( 150 )
 *    oDock:AddPanel( oPanel2 )
 *    oDock:Dock( oPanel2, HBDOCK_BOTTOM )
 *
 * Importante: oWndMain debe reenviar su WM_SIZE hacia
 * oDock:Resize( nWidth, nHeight ), a menos que la ventana
 * contenedora ya este anclada con ALIGN CLIENT / oWnd:oClient
 * en FiveWin (en ese caso FiveWin ya la redimensiona sola y
 * el propio HBDOCKCONTAINERCREATE recibe su propio WM_SIZE).
 * ---------------------------------------------------------------------------
 */

#include "hbdock.ch"

CLASS TDockManager

   DATA pManager                  // puntero HB_DOCK_MANAGER (capa C)
   DATA hWnd                      // hWnd de la ventana contenedora
   DATA oWndParent
   DATA aPanels                   // TDockPanel vivos (evita que el GC los pierda)

   METHOD New( oWndParent, hInstance )
   METHOD AddPanel( oPanel )
   METHOD Dock( oPanel, nSite )
   METHOD Undock( oPanel )
   METHOD Float( oPanel, nX, nY )
   METHOD DockBack( oPanel, nSite )
   METHOD AutoHide( oPanel )
   METHOD AutoHideRestore( oPanel )
   METHOD FindPanel( cName )
   METHOD RemovePanel( oPanel )
   METHOD Resize( nWidth, nHeight )
   METHOD Destroy()

ENDCLASS

METHOD New( oWndParent, hInstance ) CLASS TDockManager

   ::oWndParent := oWndParent
   ::aPanels    := {}

   ::pManager := HBDOCKMANAGERNEW( oWndParent:hWnd )

   HBDOCKMANAGERSETINSTANCE( ::pManager, hInstance )

   ::hWnd := HBDOCKCONTAINERCREATE( oWndParent:hWnd, hInstance, ::pManager )

return Self

METHOD AddPanel( oPanel ) CLASS TDockManager

   aadd( ::aPanels, oPanel )

   HBDOCKMANAGERADDPANEL( ::pManager, oPanel:pPanel )

return Self

METHOD Dock( oPanel, nSite ) CLASS TDockManager

   HBDOCKMANAGERDOCKPANEL( ::pManager, oPanel:pPanel, nSite )

return Self

METHOD Undock( oPanel ) CLASS TDockManager

   HBDOCKMANAGERUNDOCKPANEL( ::pManager, oPanel:pPanel )

return Self

METHOD Float( oPanel, nX, nY ) CLASS TDockManager

   HBDOCKMANAGERFLOATPANEL( ::pManager, oPanel:pPanel, nX, nY )

return Self

METHOD DockBack( oPanel, nSite ) CLASS TDockManager

   HBDOCKMANAGERDOCKBACK( ::pManager, oPanel:pPanel, nSite )

return Self

METHOD AutoHide( oPanel ) CLASS TDockManager

   HBDOCKMANAGERAUTOHIDEPANEL( ::pManager, oPanel:pPanel )

return Self

METHOD AutoHideRestore( oPanel ) CLASS TDockManager

   HBDOCKMANAGERAUTOHIDERESTORE( ::pManager, oPanel:pPanel )

return Self

METHOD FindPanel( cName ) CLASS TDockManager

   local pPanel := HBDOCKMANAGERFINDPANEL( ::pManager, cName )
   local oPanel

   if pPanel == NIL
      return NIL
   endif

   for each oPanel in ::aPanels
      if oPanel:pPanel == pPanel
         return oPanel
      endif
   next

return NIL

METHOD RemovePanel( oPanel ) CLASS TDockManager

   local nAt := ascan( ::aPanels, {| o | o == oPanel } )

   HBDOCKMANAGERREMOVEPANEL( ::pManager, oPanel:pPanel )

   if nAt > 0
      hb_adel( ::aPanels, nAt, .T. )
   endif

return Self

METHOD Resize( nWidth, nHeight ) CLASS TDockManager

   // Al redimensionar el HWND contenedor, su propio WM_SIZE
   // ya dispara internamente el recalculo de layout (ver
   // hbDockContainerProc / hbDockHostResize en la capa C).
   // No hace falta (ni conviene) llamar HBDOCKMANAGERRESIZE
   // aparte: quedaria desincronizado con el tamano real del
   // HWND.
   MoveWindow( ::hWnd, 0, 0, nWidth, nHeight, .T. )

return Self

METHOD Destroy() CLASS TDockManager

   local oPanel

   for each oPanel in ::aPanels
      oPanel:Destroy()
   next

   ::aPanels := {}

   if ::pManager != NIL
      HBDOCKMANAGERDELETE( ::pManager )
      ::pManager := NIL
   endif

return Self
