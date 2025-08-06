# 🚀 Compresor de Archivos - Versión Interactiva

## 📋 Descripción

Esta es una versión interactiva del compresor de archivos con una interfaz de consola amigable y moderna. Incluye menús, barras de progreso y estadísticas detalladas.

## ✨ Características

- 🎯 **Interfaz interactiva** con menús fáciles de usar
- 📁 **Compresión individual** de archivos
- 📂 **Compresión múltiple** de varios archivos
- 📊 **Estadísticas detalladas** de compresión
- 🔧 **Información y configuración** integrada
- 🎨 **Interfaz visual** con caracteres Unicode
- ⚡ **Compresión rápida** usando zlib
- 📈 **Ratios de compresión** en tiempo real

## 🛠️ Instalación

### Requisitos
- Compilador C++ con soporte para C++17
- Biblioteca zlib
- macOS, Linux o Windows

### Compilación

```bash
# Compilar la versión interactiva
make -f Makefile_interactive

# O usar el script
./run_interactive.sh
```

## 🚀 Uso

### Ejecutar la aplicación

```bash
# Opción 1: Usar el script
./run_interactive.sh

# Opción 2: Ejecutar directamente
./interactive_compressor

# Opción 3: Usar make
make -f Makefile_interactive run
```

### Navegación por menús

La aplicación presenta un menú principal con las siguientes opciones:

1. **📁 Comprimir un archivo** - Comprime un archivo individual
2. **📂 Comprimir múltiples archivos** - Comprime varios archivos a la vez
3. **ℹ️ Información de compresión** - Muestra información sobre el compresor
4. **⚙️ Configuración** - Muestra configuración y estadísticas
5. **🚪 Salir** - Cierra la aplicación

### Compresión de archivos

#### Archivo individual:
1. Selecciona opción 1 del menú principal
2. Ingresa la ruta completa del archivo
3. La aplicación mostrará el progreso y los resultados

#### Múltiples archivos:
1. Selecciona opción 2 del menú principal
2. Ingresa las rutas de los archivos (una por línea)
3. Presiona Enter vacío para terminar
4. La aplicación procesará todos los archivos

## 📁 Tipos de archivo soportados

### Archivos de texto (compresión optimizada):
- `.txt`, `.log`, `.csv`
- `.json`, `.xml`, `.html`
- `.css`, `.js`
- `.cpp`, `.h`, `.py`, `.java`

### Archivos binarios:
- Cualquier otro tipo de archivo

## 📊 Resultados

La aplicación muestra:
- ✅ Estado de la compresión (éxito/error)
- 📊 Tamaño original y comprimido
- 📈 Ratio de compresión en porcentaje
- 💾 Ubicación del archivo comprimido
- 📋 Resumen para múltiples archivos

## 📂 Ubicación de archivos

- **Archivos comprimidos**: `./output/`
- **Formato**: `nombre_original_compressed.extensión`

## 🔧 Algoritmo de compresión

- **Algoritmo**: zlib (DEFLATE)
- **Nivel**: Máximo (Z_BEST_COMPRESSION)
- **Optimización**: Especializada para archivos de texto y código

## 🎨 Características de la interfaz

- **Menús visuales** con bordes y emojis
- **Barras de progreso** animadas
- **Colores y símbolos** para mejor experiencia
- **Navegación intuitiva** con números
- **Mensajes informativos** claros

## 📝 Ejemplos de uso

### Compresión de un archivo de texto:
```
🎯 Selecciona una opción: 1

📝 Ingresa la ruta del archivo a comprimir: test.txt

🔨 Comprimiendo archivo...
🔄 ████████████████████ ✅

✅ Compresión exitosa!
📁 Archivo: test.txt
📊 Tamaño original: 2.45 KB
📊 Tamaño comprimido: 0.89 KB
📈 Ratio de compresión: 63.67%
💾 Archivo guardado en: output/test_compressed.txt
```

### Compresión múltiple:
```
🎯 Selecciona una opción: 2

📝 Ingresa las rutas de los archivos:
📁 Archivo 1: main.cpp
📁 Archivo 2: requirements.txt
📁 Archivo 3: [Enter]

🔨 Comprimiendo 2 archivos...

📊 Progreso: 1/2 - main.cpp
🔄 ████████████████████ ✅

📊 Progreso: 2/2 - requirements.txt
🔄 ████████████████████ ✅

📊 Resumen:
   • Archivos procesados: 2
   • Comprimidos exitosamente: 2
   • Errores: 0
   • Ratio promedio: 45.23%
```

## 🐛 Solución de problemas

### Error de compilación:
```bash
# Asegúrate de tener zlib instalado
# En macOS:
brew install zlib

# En Ubuntu/Debian:
sudo apt-get install zlib1g-dev
```

### Archivo no encontrado:
- Verifica que la ruta del archivo sea correcta
- Usa rutas absolutas si es necesario
- Asegúrate de que el archivo existe

### Error de permisos:
```bash
# Dale permisos de ejecución al script
chmod +x run_interactive.sh
```

## 📈 Comparación con otras versiones

| Característica | Consola Simple | Interactiva | GUI (Qt) |
|----------------|----------------|-------------|----------|
| Interfaz | Línea de comandos | Menús interactivos | Ventana gráfica |
| Facilidad de uso | Básica | Alta | Muy alta |
| Compresión múltiple | ❌ | ✅ | ✅ |
| Estadísticas | Básicas | Detalladas | Completas |
| Dependencias | Mínimas | Mínimas | Qt6 |
| Compilación | Simple | Simple | Compleja |

## 🤝 Contribuciones

Para contribuir al proyecto:
1. Fork el repositorio
2. Crea una rama para tu feature
3. Haz commit de tus cambios
4. Push a la rama
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo la licencia MIT. Ver el archivo LICENSE para más detalles.

## 👨‍💻 Autor

**Juan Bautista Espino**
- 🚀 Desarrollador de software
- 📧 Contacto: [tu-email@ejemplo.com]
- 🌐 GitHub: [tu-usuario]

---

**¡Disfruta comprimiendo archivos con la versión interactiva! 🎉**
