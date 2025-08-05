#!/usr/bin/env python3
"""
Script de instalación para el Compresor de Archivos
"""

import subprocess
import sys
import os

def check_python_version():
    """Verificar que Python sea 3.7 o superior"""
    if sys.version_info < (3, 7):
        print("❌ Error: Se requiere Python 3.7 o superior")
        print(f"   Versión actual: {sys.version}")
        return False
    print(f"✅ Python {sys.version.split()[0]} detectado")
    return True

def install_requirements():
    """Instalar dependencias desde requirements.txt"""
    try:
        print("📦 Instalando dependencias...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", "requirements.txt"])
        print("✅ Dependencias instaladas correctamente")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Error instalando dependencias: {e}")
        return False

def test_imports():
    """Probar que todas las dependencias se importen correctamente"""
    try:
        print("🧪 Probando importaciones...")
        import customtkinter
        from PIL import Image
        import PyPDF2
        print("✅ Todas las dependencias se importan correctamente")
        return True
    except ImportError as e:
        print(f"❌ Error importando dependencias: {e}")
        return False

def main():
    """Función principal de instalación"""
    print("🚀 Instalador del Compresor de Archivos")
    print("=" * 40)

    # Verificar Python
    if not check_python_version():
        return False

    # Instalar dependencias
    if not install_requirements():
        return False

    # Probar importaciones
    if not test_imports():
        return False

    print("\n🎉 ¡Instalación completada exitosamente!")
    print("\nPara ejecutar la aplicación:")
    print("   python main.py")
    print("\n¡Disfruta comprimiendo archivos! 🗜️")

    return True

if __name__ == "__main__":
    success = main()
    if not success:
        sys.exit(1)
