@echo off
setlocal enabledelayedexpansion

rem ---------------------------------------------------------------------
rem HBDock32 - build_hbdock32.bat
rem
rem Compila toda la libreria (src\*.c) con Borland C++ 5.5 (bcc32) y
rem arma hbdock32.lib con TLIB. Opcionalmente compila tambien la capa
rem de bindings Harbour (src\harbour\*.c) si HB_DIR apunta a una
rem instalacion valida de Harbour (necesita include\hbapi.h).
rem
rem Ajusta las tres variables de abajo a tu entorno antes de correrlo.
rem ---------------------------------------------------------------------

rem --- Carpeta de instalacion de Borland C++ 5.5 (bcc32.exe, tlib.exe) ---
set BCC_DIR=C:\Borland\Bcc55

rem --- Carpeta de instalacion de Harbour 3.2 (include\hbapi.h) ---
rem     Dejar vacio para omitir la capa de bindings Harbour y compilar
rem     solo la libreria C pura (util para probar el core sin Harbour).
set HB_DIR=C:\harbour

rem --- Nombre/ubicacion de la libreria resultante ---
set LIB_NAME=hbdock32.lib

rem ---------------------------------------------------------------------
rem No deberia hacer falta tocar nada de aca para abajo.
rem ---------------------------------------------------------------------

set ROOT_DIR=%~dp0
set OBJ_DIR=%ROOT_DIR%build\obj
set INC_DIR=%ROOT_DIR%include
set OBJLIST=%OBJ_DIR%\objlist.txt

if not exist "%BCC_DIR%\bin\bcc32.exe" (
   echo [ERROR] No se encontro bcc32.exe en "%BCC_DIR%\bin".
   echo         Edita BCC_DIR al principio de este .bat.
   exit /b 1
)

set PATH=%BCC_DIR%\bin;%PATH%
set BCC_INCLUDE=%BCC_DIR%\include
set BCC_LIB=%BCC_DIR%\lib

if exist "%OBJ_DIR%" rd /s /q "%OBJ_DIR%"
mkdir "%OBJ_DIR%"

if exist "%ROOT_DIR%%LIB_NAME%" del "%ROOT_DIR%%LIB_NAME%"

rem --- Flags de compilacion ---
rem   -c        solo compilar (no linkear)
rem   -w-       sin warnings (el proyecto ya compila sin avisos
rem             relevantes; ver nota al final si preferis verlos)
rem   -A        modo ANSI C (el proyecto es C89 puro)
rem   -O2       optimizacion
rem   -I        carpetas de include
rem   -n        carpeta de salida de los .obj
set CFLAGS=-c -w- -A -O2 -I"%INC_DIR%" -I"%BCC_INCLUDE%" -n"%OBJ_DIR%"

echo.
echo === Compilando core (src\*.c, sin src\harbour) ===
echo.

set FAIL=0

for /r "%ROOT_DIR%src" %%F in (*.c) do (
   echo %%F | findstr /I "\harbour\" >nul
   if errorlevel 1 (
      echo   %%~nxF
      bcc32 %CFLAGS% "%%F"
      if errorlevel 1 (
         echo [ERROR] Fallo compilando %%F
         set FAIL=1
      )
   )
)

if "%FAIL%"=="1" (
   echo.
   echo [ABORTADO] Hubo errores compilando el core. Revisa arriba.
   exit /b 1
)

if "%HB_DIR%"=="" (
   echo.
   echo === HB_DIR vacio: se omite la capa de bindings Harbour ===
) else (
   if not exist "%HB_DIR%\include\hbapi.h" (
      echo.
      echo [AVISO] No se encontro "%HB_DIR%\include\hbapi.h".
      echo         Se omite la capa de bindings Harbour.
      echo         Verifica HB_DIR si la necesitas.
   ) else (
      echo.
      echo === Compilando bindings Harbour (src\harbour\*.c) ===
      echo.

      set HBFLAGS=-c -w- -A -O2 -I"%INC_DIR%" -I"%HB_DIR%\include" -I"%BCC_INCLUDE%" -n"%OBJ_DIR%"

      for %%F in ("%ROOT_DIR%src\harbour\*.c") do (
         echo   %%~nxF
         bcc32 !HBFLAGS! "%%F"
         if errorlevel 1 (
            echo [ERROR] Fallo compilando %%F
            set FAIL=1
         )
      )

      if "!FAIL!"=="1" (
         echo.
         echo [ABORTADO] Hubo errores compilando los bindings Harbour.
         exit /b 1
      )
   )
)

echo.
echo === Armando %LIB_NAME% con TLIB ===
echo.

rem TLIB necesita cada .obj precedido de "+" y no acepta bien listas
rem larguisimas en la linea de comandos, asi que se arma un archivo
rem de respuesta (@objlist.txt).

if exist "%OBJLIST%" del "%OBJLIST%"

for %%O in ("%OBJ_DIR%\*.obj") do (
   echo +"%%O" >> "%OBJLIST%"
)

tlib "%ROOT_DIR%%LIB_NAME%" /C /P128 @"%OBJLIST%"

if errorlevel 1 (
   echo.
   echo [ERROR] TLIB fallo armando la libreria.
   exit /b 1
)

echo.
echo === OK: %ROOT_DIR%%LIB_NAME% generado correctamente ===
echo.

endlocal
exit /b 0

rem ---------------------------------------------------------------------
rem Notas:
rem
rem  - Este .bat se escribio siguiendo la sintaxis estandar de bcc32/
rem    tlib de Borland C++ 5.5, pero no se pudo probar contra una
rem    instalacion real (no disponible en el entorno donde se genero).
rem    Antes de confiar en el del todo, corre una primera vez y revisa
rem    que los flags (sobre todo -A y -w-) den el resultado que
rem    esperas; si preferis ver los warnings en vez de silenciarlos,
rem    cambia -w- por -w (con los mismos ~5 warnings triviales de
rem    parametro no usado que ya se conocen del build de referencia
rem    con mingw).
rem
rem  - /P128 en TLIB fija el tamano de pagina de la libreria en 128
rem    bytes; con 120 objetos no deberia hacer falta, pero evita el
rem    tipico error "library page size too small" si el proyecto
rem    sigue creciendo.
rem
rem  - La sintaxis exacta de TLIB (el "@objlist.txt" como lista de
rem    modulos a agregar) sigue la convencion mas comun que se ve en
rem    makefiles viejos de Borland, pero es la parte del toolchain
rem    con mas variantes historicas segun la version exacta de TLIB.
rem    Si tlib se queja del formato del archivo de respuesta, proba
rem    agregando "&" al final de cada linea salvo la ultima (formato
rem    de continuacion de TLIB) o consulta "tlib /?" de tu version.
rem
rem  - Si tu Harbour 3.2 esta en otra estructura de carpetas (por
rem    ejemplo include\ en un lugar distinto), ajusta HB_DIR o el
rem    -I directamente.
rem ---------------------------------------------------------------------
