#!/usr/bin/env python3
"""
Script de prueba para verificar la compresión de PDFs
"""

import os
import sys
from compressor import FileCompressor, CompressionConfig

def test_pdf_compression():
    """Test PDF compression functionality"""
    print("🧪 Probando compresión de PDFs...")

    # Create compressor
    compressor = FileCompressor()

    # Check if Ghostscript is available
    if compressor._ghostscript_available:
        print("✅ Ghostscript disponible para compresión avanzada")
    else:
        print("⚠️  Ghostscript no disponible - usando compresión básica")
        print("💡 Ejecuta './install_ghostscript.sh' para instalar Ghostscript")

    # Test with a sample PDF (if exists)
    test_pdf = "test.pdf"
    if os.path.exists(test_pdf):
        print(f"📄 Probando con: {test_pdf}")

        # Get original size
        original_size = os.path.getsize(test_pdf)
        print(f"📏 Tamaño original: {original_size / 1024:.2f} KB")

        # Compress PDF
        output_path = "test_compressed.pdf"
        try:
            result = compressor.compress_file(test_pdf, output_path)

            if result['success']:
                compressed_size = result['compressed_size']
                ratio = result['compression_ratio']

                print(f"📦 Tamaño comprimido: {compressed_size / 1024:.2f} KB")
                print(f"📉 Compresión: {ratio:.1f}%")

                if ratio > 0:
                    print("✅ Compresión exitosa!")
                else:
                    print("⚠️  No se logró comprimir el PDF")

            else:
                print("❌ Error en la compresión")

        except Exception as e:
            print(f"❌ Error: {e}")

    else:
        print("📄 No se encontró test.pdf")
        print("💡 Coloca un archivo PDF llamado 'test.pdf' en este directorio para probar")

if __name__ == "__main__":
    test_pdf_compression()
