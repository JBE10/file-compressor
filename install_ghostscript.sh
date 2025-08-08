#!/bin/bash

echo "🔧 Instalando Ghostscript para mejor compresión de PDFs..."

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew no está instalado. Instalando Homebrew primero..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Install Ghostscript
echo "📦 Instalando Ghostscript..."
brew install ghostscript

# Verify installation
if command -v gs &> /dev/null; then
    echo "✅ Ghostscript instalado exitosamente!"
    echo "📊 Versión: $(gs --version)"
    echo ""
    echo "🎉 Ahora los PDFs se comprimirán mejor usando Ghostscript!"
    echo "💡 Para usar la compresión avanzada, ejecuta tu compresor nuevamente."
else
    echo "❌ Error al instalar Ghostscript"
    exit 1
fi
