#!/bin/bash

echo "🚀 Compresor de Archivos C++"
echo "============================"

# Check if compressor executable exists
if [ ! -f "./compressor" ]; then
    echo "❌ Error: El ejecutable 'compressor' no existe."
    echo "📦 Por favor, compila primero con: make"
    exit 1
fi

# Check if file argument is provided
if [ $# -eq 0 ]; then
    echo "📝 Uso: $0 <archivo_a_comprimir>"
    echo ""
    echo "📋 Ejemplos:"
    echo "  $0 test.txt"
    echo "  $0 main.py"
    echo "  $0 requirements.txt"
    echo ""
    echo "📁 Archivos disponibles para comprimir:"
    ls -la *.txt *.py *.md *.sh *.cpp *.h *.json 2>/dev/null | head -10
    exit 1
fi

# Check if input file exists
if [ ! -f "$1" ]; then
    echo "❌ Error: El archivo '$1' no existe."
    exit 1
fi

# Run the compressor
echo "🔨 Ejecutando compresor..."
./compressor "$1"

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Compresión completada exitosamente!"
    echo "📁 Revisa el archivo comprimido en la carpeta 'output/'"
else
    echo ""
    echo "❌ Error en la compresión."
    exit 1
fi
