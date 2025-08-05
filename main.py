#!/usr/bin/env python3
"""
Compresor de Archivos - Optimizado
==================================

Una aplicación de escritorio moderna para comprimir archivos, imágenes y PDFs
con una interfaz gráfica avanzada y procesamiento paralelo.

Características principales:
- Compresión inteligente de imágenes (PNG, JPG, JPEG, BMP, TIFF, WebP)
- Compresión optimizada de PDFs
- Compresión de archivos generales (ZIP, GZIP)
- Interfaz gráfica moderna con tema oscuro
- Procesamiento paralelo para mayor velocidad
- Progreso en tiempo real con estimación de tiempo
- Estadísticas detalladas de compresión
- Configuración avanzada de calidad

Autor: Juan Bautista Espino
Versión: 2.0 - Optimizada
"""

import sys
import os
import logging
import traceback
from pathlib import Path
from typing import Optional

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('compressor.log'),
        logging.StreamHandler(sys.stdout)
    ]
)
logger = logging.getLogger(__name__)

def check_dependencies() -> bool:
    """Check if all required dependencies are available"""
    required_packages = [
        'customtkinter',
        'PIL',
        'PyPDF2',
        'tkinter'
    ]

    missing_packages = []

    for package in required_packages:
        try:
            if package == 'PIL':
                import PIL
            elif package == 'PyPDF2':
                import PyPDF2
            elif package == 'tkinter':
                import tkinter
            else:
                __import__(package)
        except ImportError:
            missing_packages.append(package)

    if missing_packages:
        logger.error(f"Missing required packages: {', '.join(missing_packages)}")
        print(f"❌ Error: Faltan dependencias requeridas: {', '.join(missing_packages)}")
        print("\n📦 Por favor, instala las dependencias ejecutando:")
        print("pip install -r requirements.txt")
        return False

    return True

def check_system_requirements() -> bool:
    """Check system requirements"""
    try:
        # Check Python version
        if sys.version_info < (3, 7):
            logger.error("Python 3.7 or higher is required")
            print("❌ Error: Se requiere Python 3.7 o superior")
            print(f"   Versión actual: {sys.version}")
            return False

        # Check available memory (optional)
        try:
            import psutil
            memory = psutil.virtual_memory()
            if memory.total < 512 * 1024 * 1024:  # 512MB
                logger.warning("Low memory detected")
                print("⚠️  Advertencia: Memoria baja detectada")
        except ImportError:
            pass  # psutil is optional

        return True

    except Exception as e:
        logger.error(f"Error checking system requirements: {e}")
        return False

def setup_environment() -> bool:
    """Setup application environment"""
    try:
        # Add current directory to Python path
        current_dir = Path(__file__).parent.absolute()
        if str(current_dir) not in sys.path:
            sys.path.insert(0, str(current_dir))

        # Create necessary directories
        output_dir = current_dir / "output"
        output_dir.mkdir(exist_ok=True)

        # Set up logging directory
        log_dir = current_dir / "logs"
        log_dir.mkdir(exist_ok=True)

        logger.info(f"Application directory: {current_dir}")
        logger.info(f"Output directory: {output_dir}")
        logger.info(f"Log directory: {log_dir}")

        return True

    except Exception as e:
        logger.error(f"Error setting up environment: {e}")
        print(f"❌ Error configurando el entorno: {e}")
        return False

def show_startup_info():
    """Display startup information"""
    print("🚀 Iniciando Compresor de Archivos - Versión Optimizada")
    print("=" * 60)
    print("📋 Características:")
    print("   • Compresión inteligente de imágenes y PDFs")
    print("   • Procesamiento paralelo para mayor velocidad")
    print("   • Interfaz moderna con tema oscuro")
    print("   • Configuración avanzada de calidad")
    print("   • Estimación de tiempo en tiempo real")
    print("=" * 60)

def main():
    """Main application entry point"""
    try:
        # Show startup info
        show_startup_info()

        # Check system requirements
        if not check_system_requirements():
            return 1

        # Setup environment
        if not setup_environment():
            return 1

        # Check dependencies
        if not check_dependencies():
            return 1

        logger.info("Starting application...")
        print("✅ Todas las verificaciones completadas exitosamente")
        print("🎯 Iniciando interfaz gráfica...")

        # Import and start GUI
        from gui import CompressorGUI

        # Create and run application
        app = CompressorGUI()
        app.run()

        logger.info("Application closed successfully")
        return 0

    except KeyboardInterrupt:
        logger.info("Application interrupted by user")
        print("\n⚠️  Aplicación interrumpida por el usuario")
        return 0

    except ImportError as e:
        logger.error(f"Import error: {e}")
        print(f"❌ Error de importación: {e}")
        print("\n📦 Por favor, instala las dependencias ejecutando:")
        print("pip install -r requirements.txt")
        return 1

    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        logger.error(f"Traceback: {traceback.format_exc()}")
        print(f"❌ Error inesperado: {e}")
        print("\n🔧 Para obtener más información, revisa el archivo 'compressor.log'")
        print("📧 Si el problema persiste, contacta al desarrollador")
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
