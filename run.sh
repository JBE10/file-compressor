#!/bin/bash

# Script para ejecutar el Compresor de Archivos
echo "🚀 Iniciando Compresor de Archivos..."

# Verificar si el entorno virtual existe
if [ ! -d "venv" ]; then
    echo "❌ Error: El entorno virtual no existe."
    echo "📦 Por favor, ejecuta primero: python3 -m venv venv"
    echo "   Luego: source venv/bin/activate && pip install -r requirements.txt"
    exit 1
fi

# Activar el entorno virtual y ejecutar la aplicación
source venv/bin/activate
python main.py 