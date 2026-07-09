/*
 * ---------------------------------------------------------------------------
 * HBDock32 - Ejemplo de uso con FiveWin/Harbour
 *
 * Ventana principal con tres paneles acoplables, al estilo
 * Visual Studio:
 *
 *    +------------------------------------------------+
 *    | [Float Project] [Dock Project] [AutoHide Output]|
 *    +----------+---------------------------------------+
 *    | Project  |                                        |
 *    | (LEFT)   |          Editor (CENTER)                |
 *    |          |                                        |
 *    |          +----------------------------------------+
 *    |          |          Output (BOTTOM)                |
 *    +----------+---------------------------------------+
 *
 * NOTA sobre el hook de resize: se usa oWnd:bResized, que es
 * la convencion habitual en FiveWin para enterarse de cuando
 * cambia el tamano de la ventana. Si tu version de FiveWin
 * expone este evento con otro nombre o firma de parametros,
 * ajustalo -- no se pudo compilar este .prg contra un
 * compilador Harbour real en el momento de escribirlo (ver
 * nota al final del archivo).
 *
 * Compilar (ejemplo con hbmk2):
 *
 *    hbmk2 hbdockdemo.prg hbdockmanager.prg hbdockpanel.prg ^
 *          -inc=..\include -L..\lib -lhbdock32 -lfivehc -lfivehb
 *
 * (ajusta rutas/nombres de librerias a tu entorno; hbdock32
 * es la libreria resultante de compilar todo src/*.c de
 * HBDock32, incluida la capa src/harbour/*.c, con Borland
 * C++ 5.5 o el compilador que estes usando).
 * ---------------------------------------------------------------------------
 */

#include "FiveWin.ch"
#include "hbdock.ch"

//------------------------------------------------------------------------//

function Main()

   local oWnd
   local oBar
   local oProjectList, oOutputList, oEditor
   local oDock
   local oPanelProject, oPanelOutput, oPanelEditor

   DEFINE WINDOW oWnd ;
      TITLE "HBDock32 - Ejemplo FiveWin/Harbour" ;
      MENU BuildMenu()

      // --- Barra con botones para probar la API en vivo ---

      DEFINE BUTTONBAR oBar OF oWnd SIZE 90, 26 2007

      DEFINE BUTTON OF oBar ;
         PROMPT "Flotar" ;
         ACTION oDock:Float( oPanelProject, 200, 200 )

      DEFINE BUTTON OF oBar ;
         PROMPT "Acoplar" ;
         ACTION oDock:DockBack( oPanelProject, HBDOCK_LEFT )

      DEFINE BUTTON OF oBar ;
         PROMPT "AutoHide" ;
         ACTION oDock:AutoHide( oPanelOutput )

      DEFINE BUTTON OF oBar ;
         PROMPT "Restaurar" ;
         ACTION oDock:AutoHideRestore( oPanelOutput )

      ON INIT ( ;

         // --- Controles de contenido reales (FiveWin) ---
         // Se crean como hijos de oWnd; HBDock32 los va a
         // reparentar dentro de su propio arbol de docking
         // al asociarlos con SetWindow().

         oProjectList := TListBox():New( 0, 0, 100, 100, oWnd,,,,, .T. ),
         oProjectList:Add( "main.prg" ),
         oProjectList:Add( "hbdockmanager.prg" ),
         oProjectList:Add( "hbdockpanel.prg" ),

         oOutputList := TListBox():New( 0, 0, 100, 100, oWnd,,,,, .T. ),
         oOutputList:Add( "Compilando..." ),
         oOutputList:Add( "0 errores, 0 advertencias" ),

         oEditor := TMultiGet():New( 0, 0, 100, 100,,, oWnd,,,,,,,, .T. ),
         oEditor:bKeyDown := nil,

         // --- Manager de docking ---

         oDock := TDockManager():New( oWnd, GetInstance() ),

         // --- Panel Editor: se acopla primero, por lo que
         //     pasa a ocupar todo el arbol (el sitio que le
         //     pidas para el primero no importa) ---

         oPanelEditor := TDockPanel():New( "EDITOR", "Editor" ),
         oPanelEditor:SetWindow( oEditor ),
         oDock:AddPanel( oPanelEditor ),
         oDock:Dock( oPanelEditor, HBDOCK_CENTER ),

         // --- Panel Project: se acopla a la izquierda ---

         oPanelProject := TDockPanel():New( "PROJECT", "Project Explorer" ),
         oPanelProject:SetWindow( oProjectList ),
         oPanelProject:SetMinSize( 120, 120 ),
         oPanelProject:SetPreferredSize( 220 ),
         oDock:AddPanel( oPanelProject ),
         oDock:Dock( oPanelProject, HBDOCK_LEFT ),

         // --- Panel Output: se acopla abajo ---

         oPanelOutput := TDockPanel():New( "OUTPUT", "Output" ),
         oPanelOutput:SetWindow( oOutputList ),
         oPanelOutput:SetMinSize( 120, 100 ),
         oPanelOutput:SetPreferredSize( 150 ),
         oDock:AddPanel( oPanelOutput ),
         oDock:Dock( oPanelOutput, HBDOCK_BOTTOM ),

         // --- Mantener el contenedor de docking del tamano
         //     del cliente de oWnd cada vez que se redimensiona ---

         oWnd:bResized := {|nWidth, nHeight| oDock:Resize( nWidth, nHeight ) }, // ver nota arriba

         oDock:Resize( oWnd:nWidth(), oWnd:nHeight() ) )

      ON RELEASE ( ;
         oDock:Destroy() )

   ACTIVATE WINDOW oWnd

return nil

//------------------------------------------------------------------------//

static function BuildMenu()

   local oMenu

   MENU oMenu

      MENUITEM "&Salir" ACTION oWnd:End()

   ENDMENU

return oMenu

//------------------------------------------------------------------------//

/*
 * Nota de verificacion:
 *
 * Este archivo no se pudo compilar contra un compilador
 * Harbour/FiveWin real (no estan disponibles en el entorno
 * donde se genero). Se escribio siguiendo la sintaxis
 * estandar de FiveWin de memoria, con el cuidado habitual,
 * pero antes de darlo por bueno conviene compilarlo una vez
 * con tu hbmk2/bcc32 y revisar en particular:
 *
 *  - El nombre y la firma exacta del hook de resize
 *    (oWnd:bResized) en tu version de FiveWin.
 *  - Los parametros exactos de TListBox():New() y
 *    TMultiGet():New() (varian bastante entre versiones
 *    de FiveWin); se dejaron con los mas comunes que se
 *    usan para crear el control como hijo de oWnd, sin
 *    ALIGN, ya que HBDock32 es quien decide su posicion
 *    y tamano una vez acoplado.
 */
