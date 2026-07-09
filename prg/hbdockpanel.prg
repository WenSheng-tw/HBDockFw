/*
 * ---------------------------------------------------------------------------
 * HBDock32 - TDockPanel
 *
 * Envoltorio OOP (estilo FiveWin) sobre un HB_DOCK_PANEL de
 * la capa C. Un TDockPanel es el panel acoplable en si; el
 * contenido real (un TBrowse, un TGet, cualquier control ya
 * creado) se asocia via SetWindow().
 * ---------------------------------------------------------------------------
 */

#include "hbdock.ch"

CLASS TDockPanel

   DATA pPanel                    // puntero HB_DOCK_PANEL (capa C)
   DATA cName
   DATA cCaption
   DATA oWnd                      // objeto FiveWin de contenido (opcional)

   METHOD New( cName, cCaption )
   METHOD SetWindow( oWnd )
   METHOD SetMinSize( nWidth, nHeight )
   METHOD SetPreferredSize( nSize )
   METHOD Caption( cCaption )
   METHOD IsFloating()
   METHOD DockSite()
   METHOD Destroy()

ENDCLASS

METHOD New( cName, cCaption ) CLASS TDockPanel

   ::pPanel   := HBDOCKPANELNEW()
   ::cName    := cName
   ::cCaption := hb_defaultValue( cCaption, cName )
   ::oWnd     := NIL

   HBDOCKPANELSETNAME( ::pPanel, ::cName )
   HBDOCKPANELSETCAPTION( ::pPanel, ::cCaption )

return Self

METHOD SetWindow( oWnd ) CLASS TDockPanel

   ::oWnd := oWnd

   HBDOCKPANELSETHWND( ::pPanel, oWnd:hWnd )

return Self

METHOD SetMinSize( nWidth, nHeight ) CLASS TDockPanel

   HBDOCKPANELSETMINSIZE( ::pPanel, nWidth, nHeight )

return Self

METHOD SetPreferredSize( nSize ) CLASS TDockPanel

   HBDOCKPANELSETPREFERREDSIZE( ::pPanel, nSize )

return Self

METHOD Caption( cCaption ) CLASS TDockPanel

   if cCaption != NIL
      ::cCaption := cCaption
      HBDOCKPANELSETCAPTION( ::pPanel, cCaption )
   endif

return ::cCaption

METHOD IsFloating() CLASS TDockPanel
return HBDOCKPANELISFLOATING( ::pPanel )

METHOD DockSite() CLASS TDockPanel
return HBDOCKPANELGETDOCKSITE( ::pPanel )

METHOD Destroy() CLASS TDockPanel

   if ::pPanel != NIL
      HBDOCKPANELDELETE( ::pPanel )
      ::pPanel := NIL
   endif

return Self
