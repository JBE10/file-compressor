# 🚀 Compresor de Archivos - Versión Simple

Un compresor de archivos rápido y eficiente escrito en C++ que genera archivos ZIP optimizados.

## ✨ Características

- **Compresión ZIP real**: Genera archivos ZIP estándar que se pueden abrir con cualquier programa
- **Optimización de PDFs**: Los PDFs se comprimen y se guardan en archivos ZIP con sufijo "_optimized"
- **Alta compresión**: Usa el algoritmo zlib con máxima compresión
- **Fácil de usar**: Interfaz de línea de comandos simple
- **Multiplataforma**: Funciona en macOS, Linux y Windows

## 🛠️ Instalación

### Prerrequisitos

- Compilador C++ con soporte para C++17
- Librería zlib
- Librería libzip

### En macOS (con Homebrew)

```bash
# Instalar dependencias
brew install libzip

# Compilar el compresor
g++ src/simple_compressor.cpp -o simple_compressor -std=c++17 -lz -lzip -I/opt/homebrew/include -L/opt/homebrew/lib

# Hacer ejecutable el script
chmod +x compress.sh
```

## 📖 Uso

### Uso directo del compresor

```bash
# Comprimir un archivo
./simple_compressor archivo.txt comprimido

# Comprimir un PDF
./simple_compressor documento.pdf optimizado
```

### Uso con el script (recomendado)

```bash
# Comprimir un archivo
./compress.sh archivo.txt

# Comprimir múltiples archivos
./compress.sh archivo1.txt archivo2.pdf imagen.jpg

# Comprimir todos los archivos de un tipo
./compress.sh *.pdf
```

## 📊 Ejemplos de uso

### Comprimir un archivo de texto

```bash
./compress.sh test.txt
```

**Salida:**
```
🚀 Compresor de Archivos - Versión Simple
==========================================
📁 Archivos a comprimir:
   ✅ test.txt

🔄 Comprimiendo: test.txt
Comprimiendo: test.txt
✅ Compresión exitosa!
📁 Archivo original: test.txt (1544 bytes)
📦 Archivo comprimido: output/test.zip (522 bytes)
📊 Ratio de compresión: 66.2%
✅ test.txt comprimido exitosamente

🎯 Compresión completada!
📂 Los archivos comprimidos están en el directorio 'output/'

📊 Resumen de archivos generados:
-rw-r--r--@ 1 usuario  staff  522 Aug  7 12:46 output/test.zip
```

### Comprimir un PDF

```bash
./compress.sh documento.pdf
```

**Resultado:** Se crea `output/documento_optimized.zip` que contiene el PDF comprimido.

## 📁 Estructura de archivos

```
Compresor de archivos/
├── src/
│   └── simple_compressor.cpp    # Código fuente del compresor
├── output/                      # Directorio donde se guardan los archivos comprimidos
├── simple_compressor            # Ejecutable del compresor
├── compress.sh                  # Script de compresión
└── README_COMPRESSOR.md         # Este archivo
```

## 🔧 Tipos de archivos soportados

- **Archivos de texto**: .txt, .log, .csv, .json, .xml, .html, .css, .js, .cpp, .h, .py, .java
- **PDFs**: .pdf (se optimizan especialmente)
- **Archivos binarios**: Cualquier otro tipo de archivo

## 📈 Rendimiento

El compresor utiliza:
- **zlib** para la compresión de datos
- **libzip** para crear archivos ZIP estándar
- **Compresión máxima** (Z_BEST_COMPRESSION) para obtener el mejor ratio

### Ratios típicos de compresión:
- **Archivos de texto**: 60-80% de reducción
- **PDFs**: 20-50% de reducción (dependiendo del contenido)
- **Archivos binarios**: 10-40% de reducción

## 🚨 Solución de problemas

### Error: "library 'zip' not found"
```bash
# Instalar libzip
brew install libzip

# Recompilar con el path correcto
g++ src/simple_compressor.cpp -o simple_compressor -std=c++17 -lz -lzip -I/opt/homebrew/include -L/opt/homebrew/lib
```

### Error: "No se pudo crear el archivo ZIP"
- Verificar que tienes permisos de escritura en el directorio
- Asegurarte de que hay espacio suficiente en disco

### Error: "No se pudo abrir el archivo de entrada"
- Verificar que el archivo existe
- Comprobar que tienes permisos de lectura

## 🔄 Comparación con la versión anterior

| Característica | Versión Anterior | Nueva Versión |
|----------------|------------------|---------------|
| Formato de salida | .compressed | .zip |
| Compatibilidad | Solo con el programa | Estándar universal |
| Optimización PDF | No | Sí |
| Interfaz | GUI compleja | Línea de comandos simple |
| Dependencias | Qt5 + múltiples librerías | Solo zlib + libzip |

## 📝 Licencia

Este proyecto es de código abierto y está disponible bajo la licencia MIT.

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Por favor, abre un issue o envía un pull request.

---

**¡Disfruta comprimiendo tus archivos! 🎉**
