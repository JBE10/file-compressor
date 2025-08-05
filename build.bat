@echo off
REM Script de compilación para Windows - Compresor de Archivos C++
REM Autor: Juan Bautista Espino

setlocal enabledelayedexpansion

REM Colores para output (Windows 10+)
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM Función para imprimir mensajes
:print_message
echo %BLUE%[INFO]%NC% %~1
goto :eof

:print_success
echo %GREEN%[SUCCESS]%NC% %~1
goto :eof

:print_warning
echo %YELLOW%[WARNING]%NC% %~1
goto :eof

:print_error
echo %RED%[ERROR]%NC% %~1
goto :eof

REM Verificar argumentos
set "clean_build=false"
set "debug_mode=false"
set "install_flag=false"

:parse_args
if "%~1"=="" goto :main
if "%~1"=="-h" goto :show_usage
if "%~1"=="--help" goto :show_usage
if "%~1"=="-c" set "clean_build=true"
if "%~1"=="--clean" set "clean_build=true"
if "%~1"=="-d" set "debug_mode=true"
if "%~1"=="--debug" set "debug_mode=true"
if "%~1"=="-i" set "install_flag=true"
if "%~1"=="--install" set "install_flag=true"
shift
goto :parse_args

:show_usage
echo Uso: %0 [OPCIONES]
echo.
echo Opciones:
echo   -h, --help     Mostrar esta ayuda
echo   -c, --clean    Limpiar build anterior antes de compilar
echo   -d, --debug    Compilar en modo debug
echo   -i, --install  Instalar después de compilar
echo.
echo Ejemplos:
echo   %0              # Compilación normal
echo   %0 --clean      # Limpiar y compilar
echo   %0 --debug      # Compilar en modo debug
exit /b 0

:main
call :print_message "Iniciando compilación del Compresor de Archivos - C++"
call :print_message "=================================================="

REM Verificar dependencias
call :check_dependencies
if errorlevel 1 exit /b 1

REM Limpiar build si se solicita
if "%clean_build%"=="true" (
    call :clean_build
)

REM Crear directorio build
call :create_build_dir
if errorlevel 1 exit /b 1

REM Configurar CMake
call :configure_cmake
if errorlevel 1 exit /b 1

REM Compilar
call :compile_project
if errorlevel 1 exit /b 1

REM Verificar ejecutable
call :verify_executable
if errorlevel 1 exit /b 1

call :print_success "¡Compilación completada!"
call :print_message "Para ejecutar la aplicación:"
call :print_message "  cd build ^&^& FileCompressor.exe"

REM Instalar si se solicita
if "%install_flag%"=="true" (
    call :print_message "Instalando aplicación..."
    cd build
    cmake --build . --target install
    if errorlevel 1 (
        call :print_error "Error durante la instalación"
        exit /b 1
    )
    call :print_success "Aplicación instalada exitosamente"
)

exit /b 0

:check_dependencies
call :print_message "Verificando dependencias..."

REM Verificar CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    call :print_error "CMake no está instalado. Por favor instálalo primero."
    call :print_message "Descarga desde: https://cmake.org/download/"
    exit /b 1
)
for /f "tokens=*" %%i in ('cmake --version 2^>^&1') do set "cmake_version=%%i"
call :print_success "CMake encontrado: %cmake_version%"

REM Verificar Visual Studio
where cl >nul 2>&1
if errorlevel 1 (
    call :print_error "Visual Studio no encontrado o no configurado."
    call :print_message "Por favor ejecuta 'Developer Command Prompt for VS'"
    call :print_message "o configura las variables de entorno de Visual Studio"
    exit /b 1
)
call :print_success "Visual Studio encontrado"

REM Verificar Qt6
if not defined QTDIR (
    call :print_warning "QTDIR no está configurado. Buscando Qt6..."
    for /d %%i in ("C:\Qt\6.*") do (
        if exist "%%i\bin\qmake.exe" (
            set "QTDIR=%%i"
            call :print_success "Qt6 encontrado en: !QTDIR!"
            goto :qt_found
        )
    )
    call :print_error "Qt6 no está instalado o no se encuentra."
    call :print_message "Por favor instala Qt6 desde: https://www.qt.io/download/"
    exit /b 1
)
:qt_found

REM Verificar vcpkg (opcional)
if exist "vcpkg\scripts\buildsystems\vcpkg.cmake" (
    call :print_success "vcpkg encontrado"
) else (
    call :print_warning "vcpkg no encontrado. Se usará instalación manual de dependencias."
)

exit /b 0

:clean_build
if exist "build" (
    call :print_message "Limpiando build anterior..."
    rmdir /s /q build
)
exit /b 0

:create_build_dir
call :print_message "Creando directorio build..."
if not exist "build" mkdir build
cd build
exit /b 0

:configure_cmake
call :print_message "Configurando CMake..."

set "cmake_args=-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON"

REM Agregar toolchain de vcpkg si existe
if exist "..\vcpkg\scripts\buildsystems\vcpkg.cmake" (
    set "cmake_args=%cmake_args% -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake"
    call :print_message "Usando vcpkg toolchain"
)

cmake %cmake_args% ..
if errorlevel 1 (
    call :print_error "Error configurando CMake"
    exit /b 1
)
call :print_success "CMake configurado exitosamente"
exit /b 0

:compile_project
call :print_message "Compilando proyecto..."

REM Obtener número de núcleos
for /f "tokens=*" %%i in ('wmic cpu get NumberOfCores /value ^| find "="') do set "%%i"
set "cores=%NumberOfCores%"
if "%cores%"=="" set "cores=4"

call :print_message "Usando %cores% núcleos para compilación"

cmake --build . --config Release --parallel %cores%
if errorlevel 1 (
    call :print_error "Error durante la compilación"
    exit /b 1
)
call :print_success "Compilación completada exitosamente"
exit /b 0

:verify_executable
if exist "Release\FileCompressor.exe" (
    call :print_success "Ejecutable creado: FileCompressor.exe"
    for %%A in ("Release\FileCompressor.exe") do set "size=%%~zA"
    call :print_message "Tamaño: %size% bytes"
) else if exist "FileCompressor.exe" (
    call :print_success "Ejecutable creado: FileCompressor.exe"
    for %%A in ("FileCompressor.exe") do set "size=%%~zA"
    call :print_message "Tamaño: %size% bytes"
) else (
    call :print_error "No se encontró el ejecutable FileCompressor.exe"
    exit /b 1
)
exit /b 0
