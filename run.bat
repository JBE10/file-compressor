@echo off
echo 🚀 Iniciando Compresor de Archivos...

REM Verificar si el entorno virtual existe
if not exist "venv" (
    echo ❌ Error: El entorno virtual no existe.
    echo 📦 Por favor, ejecuta primero: python -m venv venv
    echo    Luego: venv\Scripts\activate && pip install -r requirements.txt
    pause
    exit /b 1
)

REM Activar el entorno virtual y ejecutar la aplicación
call venv\Scripts\activate
python main.py
pause
