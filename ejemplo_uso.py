#!/usr/bin/env python3
"""
Ejemplo de uso del Compresor de Archivos
========================================

Este script demuestra cómo usar la clase FileCompressor programáticamente
sin la interfaz gráfica.
"""

import os
import sys
from compressor import FileCompressor

def progress_callback(message, percentage):
    """Callback para mostrar el progreso"""
    print(f"[{percentage:3.0f}%] {message}")

def ejemplo_compresion_individual():
    """Ejemplo de compresión de un archivo individual"""
    print("📁 Ejemplo: Compresión de archivo individual")
    print("-" * 50)

    # Crear compresor
    compressor = FileCompressor(progress_callback=progress_callback)

    # Archivo de ejemplo (crear uno si no existe)
    archivo_ejemplo = "archivo_ejemplo.txt"
    if not os.path.exists(archivo_ejemplo):
        with open(archivo_ejemplo, "w") as f:
            f.write("Este es un archivo de ejemplo para probar la compresión.\n" * 1000)
        print(f"✅ Archivo de ejemplo creado: {archivo_ejemplo}")

    # Comprimir archivo
    resultado = compressor.compress_file(
        input_path=archivo_ejemplo,
        output_path="archivo_ejemplo.zip",
        compression_type="zip"
    )

    if resultado['success']:
        print(f"✅ Compresión exitosa!")
        print(f"   Tamaño original: {resultado['original_size']} bytes")
        print(f"   Tamaño comprimido: {resultado['compressed_size']} bytes")
        print(f"   Compresión: {resultado['compression_ratio']:.1f}%")
    else:
        print("❌ Error en la compresión")

def ejemplo_compresion_multiple():
    """Ejemplo de compresión de múltiples archivos"""
    print("\n📁 Ejemplo: Compresión de múltiples archivos")
    print("-" * 50)

    # Crear archivos de ejemplo
    archivos_ejemplo = []
    for i in range(3):
        nombre = f"archivo_ejemplo_{i+1}.txt"
        with open(nombre, "w") as f:
            f.write(f"Contenido del archivo {i+1}\n" * 500)
        archivos_ejemplo.append(nombre)

    print(f"✅ Archivos de ejemplo creados: {len(archivos_ejemplo)}")

    # Crear directorio de salida
    directorio_salida = "archivos_comprimidos"
    os.makedirs(directorio_salida, exist_ok=True)

    # Comprimir múltiples archivos
    compressor = FileCompressor(progress_callback=progress_callback)
    resultados = compressor.compress_multiple_files(
        file_paths=archivos_ejemplo,
        output_dir=directorio_salida,
        compression_type="zip"
    )

    # Mostrar resultados
    exitosos = sum(1 for r in resultados if r['success'])
    print(f"\n📊 Resultados:")
    print(f"   Archivos procesados: {exitosos}/{len(resultados)}")

    for resultado in resultados:
        if resultado['success']:
            print(f"   ✅ {resultado['filename']}: {resultado['compression_ratio']:.1f}%")
        else:
            print(f"   ❌ {resultado['filename']}: {resultado['error']}")

def ejemplo_diferentes_tipos():
    """Ejemplo con diferentes tipos de compresión"""
    print("\n📁 Ejemplo: Diferentes tipos de compresión")
    print("-" * 50)

    # Crear archivo de ejemplo
    archivo = "datos_ejemplo.txt"
    with open(archivo, "w") as f:
        f.write("Datos de ejemplo para comparar tipos de compresión.\n" * 2000)

    compressor = FileCompressor(progress_callback=progress_callback)

    # Probar ZIP
    print("🗜️ Probando compresión ZIP...")
    resultado_zip = compressor.compress_file(archivo, "datos_ejemplo.zip", "zip")

    # Probar GZIP
    print("🗜️ Probando compresión GZIP...")
    resultado_gzip = compressor.compress_file(archivo, "datos_ejemplo.gz", "gzip")

    # Comparar resultados
    print(f"\n📊 Comparación de compresión:")
    print(f"   ZIP:  {resultado_zip['compression_ratio']:.1f}%")
    print(f"   GZIP: {resultado_gzip['compression_ratio']:.1f}%")

def limpiar_archivos_ejemplo():
    """Limpiar archivos de ejemplo creados"""
    print("\n🧹 Limpiando archivos de ejemplo...")

    archivos_a_eliminar = [
        "archivo_ejemplo.txt",
        "archivo_ejemplo.zip",
        "archivo_ejemplo_1.txt",
        "archivo_ejemplo_2.txt",
        "archivo_ejemplo_3.txt",
        "datos_ejemplo.txt",
        "datos_ejemplo.zip",
        "datos_ejemplo.gz"
    ]

    for archivo in archivos_a_eliminar:
        if os.path.exists(archivo):
            os.remove(archivo)
            print(f"   Eliminado: {archivo}")

    # Eliminar directorio si está vacío
    if os.path.exists("archivos_comprimidos"):
        try:
            os.rmdir("archivos_comprimidos")
            print("   Eliminado: archivos_comprimidos/")
        except OSError:
            print("   Directorio archivos_comprimidos/ no está vacío")

def main():
    """Función principal del ejemplo"""
    print("🚀 Ejemplos de uso del Compresor de Archivos")
    print("=" * 60)

    try:
        # Ejecutar ejemplos
        ejemplo_compresion_individual()
        ejemplo_compresion_multiple()
        ejemplo_diferentes_tipos()

        print("\n🎉 ¡Todos los ejemplos completados exitosamente!")

        # Preguntar si limpiar archivos
        respuesta = input("\n¿Deseas eliminar los archivos de ejemplo? (s/n): ")
        if respuesta.lower() in ['s', 'si', 'sí', 'y', 'yes']:
            limpiar_archivos_ejemplo()
            print("✅ Archivos de ejemplo eliminados")

    except Exception as e:
        print(f"❌ Error durante la ejecución: {e}")
        return False

    return True

if __name__ == "__main__":
    success = main()
    if not success:
        sys.exit(1)
