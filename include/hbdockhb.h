#ifndef HBDOCKHB_H
#define HBDOCKHB_H

/*
 * Capa de bindings Harbour para HBDock32.
 *
 * Convenciones:
 *  - HB_DOCK_MANAGER * / HB_DOCK_PANEL * / HWND / HINSTANCE
 *    viajan como punteros opacos (hb_retptr / hb_parptr).
 *  - Como distintas versiones de FiveWin representan hWnd
 *    como numerico (mas viejas) o como puntero (mas nuevas,
 *    ya que en Win64 un numerico Harbour no siempre alcanza
 *    para guardar un puntero), hb_dockParamHandle() acepta
 *    ambos casos de forma transparente.
 *  - HB_DOCK_SITE se pasa como numerico simple (ver hbdock.ch
 *    para las constantes HBDOCK_LEFT / RIGHT / TOP / BOTTOM /
 *    CENTER / NONE).
 */

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Lee el parametro iParam como un handle (HWND, HINSTANCE,
 * o cualquier puntero Win32), acepte o no numerico. */
void * hb_dockParamHandle( int iParam );

/* Devuelve un handle como resultado, de la forma mas segura
 * (siempre como puntero, ver comentario arriba). */
void hb_dockRetHandle( void * handle );

#ifdef __cplusplus
}
#endif

#endif
