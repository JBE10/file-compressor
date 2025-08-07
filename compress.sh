#!/bin/bash

echo "🚀 Compresor de Archivos - Versión Simple"
echo "=========================================="

# Verificar que se proporcionaron argumentos
if [ $# -lt 1 ]; then
    echo "❌ Error: Debes especificar al menos un archivo para comprimir"
    echo ""
    echo "📖 Uso:"
    echo "   $0 <archivo1> [archivo2] [archivo3] ..."
    echo ""
    echo "📝 Ejemplos:"
    echo "   $0 documento.pdf"
    echo "   $0 imagen.jpg texto.txt"
    echo "   $0 *.pdf"
    echo ""
    exit 1
fi

# Crear directorio de salida si no existe
mkdir -p output

echo "📁 Archivos a comprimir:"
for file in "$@"; do
    if [ -f "$file" ]; then
        echo "   ✅ $file"
    else
        echo "   ❌ $file (no encontrado)"
    fi
done
echo ""

# Comprimir cada archivo
for file in "$@"; do
    if [ -f "$file" ]; then
        echo "🔄 Comprimiendo: $file"

        # Obtener el nombre base del archivo
        filename=$(basename "$file")
        name_without_ext="${filename%.*}"

        # Comprimir usando el compresor C++
        ./simple_compressor "$file" "output/$name_without_ext"

        if [ $? -eq 0 ]; then
            echo "✅ $file comprimido exitosamente"
        else
            echo "❌ Error al comprimir $file"
        fi
        echo ""
    fi
done

echo "🎯 Compresión completada!"
echo "📂 Los archivos comprimidos están en el directorio 'output/'"
echo ""
echo "📊 Resumen de archivos generados:"
ls -la output/*.zip 2>/dev/null || echo "   No se encontraron archivos ZIP"
