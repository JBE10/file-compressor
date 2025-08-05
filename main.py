#!/usr/bin/env python3
"""
Compresor de Archivos
=====================

Una aplicación de escritorio para comprimir archivos, imágenes y PDFs
con una interfaz gráfica moderna y fácil de usar.

Características:
- Compresión de imágenes (PNG, JPG, JPEG, BMP, TIFF)
- Compresión de PDFs
- Compresión de archivos generales (ZIP, GZIP)
- Interfaz gráfica moderna con tema oscuro
- Progreso en tiempo real
- Estadísticas detalladas de compresión

Autor: Juan Bautista Espino
"""

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from gui import CompressorGUI
    print("Iniciando Compresor de Archivos...")
    print("=" * 40)

    # Start the application
    app = CompressorGUI()
    app.run()

except ImportError as e:
    print(f"Error: No se pudieron importar las dependencias necesarias.")
    print(f"Detalles: {e}")
    print("\nPor favor, instala las dependencias ejecutando:")
    print("pip install -r requirements.txt")

except Exception as e:
    print(f"Error inesperado: {e}")
    print("Por favor, verifica que todas las dependencias estén instaladas correctamente.")
