#!/bin/bash

# Script de compilación para el Compresor de Archivos - C++
# Autor: Juan Bautista Espino

set -e  # Exit on error

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Función para imprimir mensajes
print_message() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Función para verificar dependencias
check_dependencies() {
    print_message "Verificando dependencias..."

    # Verificar CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake no está instalado. Por favor instálalo primero."
        exit 1
    fi

    # Verificar compilador C++
    if command -v g++ &> /dev/null; then
        COMPILER="g++"
        print_success "GCC encontrado: $(g++ --version | head -n1)"
    elif command -v clang++ &> /dev/null; then
        COMPILER="clang++"
        print_success "Clang encontrado: $(clang++ --version | head -n1)"
    else
        print_error "No se encontró un compilador C++ compatible (g++ o clang++)"
        exit 1
    fi

    # Verificar Qt6
    if ! pkg-config --exists Qt6Core; then
        print_warning "Qt6 no encontrado. Intentando encontrar Qt6..."
        if [ -d "/usr/lib/qt6" ] || [ -d "/opt/qt6" ]; then
            print_success "Qt6 encontrado en el sistema"
        else
            print_error "Qt6 no está instalado. Por favor instálalo primero."
            print_message "En Ubuntu/Debian: sudo apt install qt6-base-dev"
            print_message "En macOS: brew install qt6"
            exit 1
        fi
    else
        print_success "Qt6 encontrado: $(pkg-config --modversion Qt6Core)"
    fi

    # Verificar librerías del sistema
    local missing_libs=()

    if ! pkg-config --exists zlib; then
        missing_libs+=("zlib")
    fi

    if ! pkg-config --exists libpng; then
        missing_libs+=("libpng")
    fi

    if ! pkg-config --exists libjpeg; then
        missing_libs+=("libjpeg")
    fi

    if [ ${#missing_libs[@]} -ne 0 ]; then
        print_warning "Librerías faltantes: ${missing_libs[*]}"
        print_message "Instalando librerías faltantes..."

        if command -v apt &> /dev/null; then
            sudo apt update
            sudo apt install -y libz-dev libpng-dev libjpeg-dev
        elif command -v brew &> /dev/null; then
            brew install zlib libpng libjpeg
        else
            print_error "No se pudo instalar las librerías automáticamente"
            print_message "Por favor instala manualmente: ${missing_libs[*]}"
            exit 1
        fi
    else
        print_success "Todas las librerías del sistema están disponibles"
    fi
}

# Función para limpiar build anterior
clean_build() {
    if [ -d "build" ]; then
        print_message "Limpiando build anterior..."
        rm -rf build
    fi
}

# Función para crear directorio build
create_build_dir() {
    print_message "Creando directorio build..."
    mkdir -p build
    cd build
}

# Función para configurar CMake
configure_cmake() {
    print_message "Configurando CMake..."

    local cmake_args=(
        "-DCMAKE_BUILD_TYPE=Release"
        "-DCMAKE_CXX_STANDARD=17"
        "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
    )

    # Agregar toolchain de vcpkg si existe
    if [ -f "../vcpkg/scripts/buildsystems/vcpkg.cmake" ]; then
        cmake_args+=("-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake")
        print_message "Usando vcpkg toolchain"
    fi

    cmake "${cmake_args[@]}" ..

    if [ $? -eq 0 ]; then
        print_success "CMake configurado exitosamente"
    else
        print_error "Error configurando CMake"
        exit 1
    fi
}

# Función para compilar
compile_project() {
    print_message "Compilando proyecto..."

    # Obtener número de núcleos
    local cores=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

    print_message "Usando $cores núcleos para compilación"

    if make -j$cores; then
        print_success "Compilación completada exitosamente"
    else
        print_error "Error durante la compilación"
        exit 1
    fi
}

# Función para verificar el ejecutable
verify_executable() {
    if [ -f "FileCompressor" ]; then
        print_success "Ejecutable creado: FileCompressor"
        print_message "Tamaño: $(du -h FileCompressor | cut -f1)"

        # Verificar que el ejecutable funciona
        if ./FileCompressor --help &> /dev/null || ./FileCompressor --version &> /dev/null; then
            print_success "Ejecutable verificado correctamente"
        else
            print_warning "No se pudo verificar el ejecutable (esto es normal si no acepta argumentos)"
        fi
    else
        print_error "No se encontró el ejecutable FileCompressor"
        exit 1
    fi
}

# Función para mostrar información de uso
show_usage() {
    echo "Uso: $0 [OPCIONES]"
    echo ""
    echo "Opciones:"
    echo "  -h, --help     Mostrar esta ayuda"
    echo "  -c, --clean    Limpiar build anterior antes de compilar"
    echo "  -d, --debug    Compilar en modo debug"
    echo "  -i, --install  Instalar después de compilar"
    echo ""
    echo "Ejemplos:"
    echo "  $0              # Compilación normal"
    echo "  $0 --clean      # Limpiar y compilar"
    echo "  $0 --debug      # Compilar en modo debug"
}

# Función principal
main() {
    local clean_build_flag=false
    local debug_mode=false
    local install_flag=false

    # Parsear argumentos
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -c|--clean)
                clean_build_flag=true
                shift
                ;;
            -d|--debug)
                debug_mode=true
                shift
                ;;
            -i|--install)
                install_flag=true
                shift
                ;;
            *)
                print_error "Opción desconocida: $1"
                show_usage
                exit 1
                ;;
        esac
    done

    print_message "Iniciando compilación del Compresor de Archivos - C++"
    print_message "=================================================="

    # Verificar dependencias
    check_dependencies

    # Limpiar build si se solicita
    if [ "$clean_build_flag" = true ]; then
        clean_build
    fi

    # Crear directorio build
    create_build_dir

    # Configurar CMake
    configure_cmake

    # Compilar
    compile_project

    # Verificar ejecutable
    verify_executable

    print_success "¡Compilación completada!"
    print_message "Para ejecutar la aplicación:"
    print_message "  cd build && ./FileCompressor"

    # Instalar si se solicita
    if [ "$install_flag" = true ]; then
        print_message "Instalando aplicación..."
        if sudo make install; then
            print_success "Aplicación instalada exitosamente"
        else
            print_error "Error durante la instalación"
            exit 1
        fi
    fi
}

# Ejecutar función principal
main "$@"
