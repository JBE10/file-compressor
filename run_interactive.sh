#!/bin/bash

echo "🚀 Compresor de Archivos Interactivo"
echo "===================================="

# Check if interactive compressor executable exists
if [ ! -f "./interactive_compressor" ]; then
    echo "❌ Error: El ejecutable 'interactive_compressor' no existe."
    echo "📦 Por favor, compila primero con: make -f Makefile_interactive"
    exit 1
fi

# Create output directory if it doesn't exist
if [ ! -d "output" ]; then
    mkdir -p output
    echo "📁 Carpeta 'output' creada."
fi

echo "✅ Ejecutando compresor interactivo..."
echo ""

# Run the interactive compressor
./interactive_compressor
