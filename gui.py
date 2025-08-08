import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import customtkinter as ctk
import os
import threading
import time
from typing import List, Optional
from pathlib import Path
from compressor import FileCompressor, CompressionConfig

class CompressorGUI:
    def __init__(self):
        # Configure CustomTkinter
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")

        # Create main window
        self.root = ctk.CTk()
        self.root.title("Compresor de Archivos - Optimizado")
        self.root.geometry("900x700")
        self.root.resizable(True, True)

        # Set minimum window size
        self.root.minsize(800, 600)

        # Variables
        self.selected_files: List[str] = []
        self.output_directory: str = ""
        self.compression_type = tk.StringVar(value="zip")
        self.compression_config = CompressionConfig()
        self.is_compressing = False

        # Performance tracking
        self.start_time: Optional[float] = None
        self.compression_stats = {}

        # Create GUI elements
        self.create_widgets()

        # Bind window events
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

    def create_widgets(self):
        """Create all GUI widgets with improved layout"""
        # Create main container frame
        main_container = ctk.CTkFrame(self.root)
        main_container.pack(fill="both", expand=True, padx=10, pady=10)

        # Create scrollable frame using CustomTkinter's built-in scrollable frame
        self.scrollable_frame = ctk.CTkScrollableFrame(main_container)
        self.scrollable_frame.pack(fill="both", expand=True)

        # Title
        title_label = ctk.CTkLabel(
            self.scrollable_frame,
            text="Compresor de Archivos - Optimizado",
            font=ctk.CTkFont(size=28, weight="bold")
        )
        title_label.pack(pady=(20, 10))

        # Subtitle
        subtitle_label = ctk.CTkLabel(
            self.scrollable_frame,
            text="Compresión inteligente con procesamiento paralelo",
            font=ctk.CTkFont(size=14),
            text_color="gray"
        )
        subtitle_label.pack(pady=(0, 30))

        # File selection section
        self.create_file_selection_section(self.scrollable_frame)

        # Compression options section
        self.create_compression_options_section(self.scrollable_frame)

        # Progress section
        self.create_progress_section(self.scrollable_frame)

        # Results section
        self.create_results_section(self.scrollable_frame)

        # Control buttons
        self.create_control_buttons(self.scrollable_frame)

        # Status bar
        self.create_status_bar()

    def create_file_selection_section(self, parent):
        """Create file selection widgets with improved functionality"""
        # File selection frame
        file_frame = ctk.CTkFrame(parent)
        file_frame.pack(fill="x", padx=20, pady=10)

        # Section title with icon
        section_title = ctk.CTkLabel(
            file_frame,
            text="📁 Selección de Archivos",
            font=ctk.CTkFont(size=18, weight="bold")
        )
        section_title.pack(pady=(15, 10))

        # File info display
        self.file_info_label = ctk.CTkLabel(
            file_frame,
            text="No hay archivos seleccionados",
            text_color="gray",
            font=ctk.CTkFont(size=12)
        )
        self.file_info_label.pack(pady=(0, 10))

        # File list with improved styling
        list_frame = ctk.CTkFrame(file_frame)
        list_frame.pack(fill="x", padx=20, pady=5)

        self.file_listbox = tk.Listbox(
            list_frame,
            height=8,
            bg="#2b2b2b",
            fg="white",
            selectmode=tk.EXTENDED,
            font=("Consolas", 10),
            relief="flat",
            bd=0,
            highlightthickness=1,
            highlightcolor="#1f538d",
            highlightbackground="#2b2b2b"
        )
        self.file_listbox.pack(fill="both", expand=True, padx=5, pady=5)

        # File buttons frame
        file_buttons_frame = ctk.CTkFrame(file_frame)
        file_buttons_frame.pack(fill="x", padx=20, pady=10)

        # Add files button
        add_files_btn = ctk.CTkButton(
            file_buttons_frame,
            text="📂 Agregar Archivos",
            command=self.add_files,
            width=140,
            height=35
        )
        add_files_btn.pack(side="left", padx=5)

        # Add folder button
        add_folder_btn = ctk.CTkButton(
            file_buttons_frame,
            text="📁 Agregar Carpeta",
            command=self.add_folder,
            width=140,
            height=35
        )
        add_folder_btn.pack(side="left", padx=5)

        # Clear files button
        clear_files_btn = ctk.CTkButton(
            file_buttons_frame,
            text="🗑️ Limpiar Lista",
            command=self.clear_files,
            width=140,
            height=35,
            fg_color="red",
            hover_color="darkred"
        )
        clear_files_btn.pack(side="left", padx=5)

        # Output directory selection
        output_frame = ctk.CTkFrame(file_frame)
        output_frame.pack(fill="x", padx=20, pady=10)

        output_label = ctk.CTkLabel(
            output_frame,
            text="📂 Directorio de Salida:",
            font=ctk.CTkFont(size=14, weight="bold")
        )
        output_label.pack(anchor="w", padx=10, pady=5)

        self.output_path_label = ctk.CTkLabel(
            output_frame,
            text="No seleccionado",
            text_color="gray",
            font=ctk.CTkFont(size=12)
        )
        self.output_path_label.pack(anchor="w", padx=10, pady=2)

        select_output_btn = ctk.CTkButton(
            output_frame,
            text="📁 Seleccionar Directorio",
            command=self.select_output_directory,
            width=180,
            height=35
        )
        select_output_btn.pack(pady=10)

    def create_compression_options_section(self, parent):
        """Create compression options widgets with advanced settings"""
        # Options frame
        options_frame = ctk.CTkFrame(parent)
        options_frame.pack(fill="x", padx=20, pady=10)

        # Section title
        section_title = ctk.CTkLabel(
            options_frame,
            text="⚙️ Opciones de Compresión",
            font=ctk.CTkFont(size=18, weight="bold")
        )
        section_title.pack(pady=(15, 10))

        # Compression type selection
        compression_frame = ctk.CTkFrame(options_frame)
        compression_frame.pack(fill="x", padx=20, pady=10)

        compression_label = ctk.CTkLabel(
            compression_frame,
            text="Tipo de Compresión:",
            font=ctk.CTkFont(size=14, weight="bold")
        )
        compression_label.pack(anchor="w", padx=10, pady=5)

        # Radio buttons for compression type
        zip_radio = ctk.CTkRadioButton(
            compression_frame,
            text="ZIP (Recomendado) - Mejor compatibilidad",
            variable=self.compression_type,
            value="zip"
        )
        zip_radio.pack(anchor="w", padx=10, pady=2)

        gzip_radio = ctk.CTkRadioButton(
            compression_frame,
            text="GZIP - Mayor compresión",
            variable=self.compression_type,
            value="gzip"
        )
        gzip_radio.pack(anchor="w", padx=10, pady=2)

        # Advanced settings
        advanced_frame = ctk.CTkFrame(options_frame)
        advanced_frame.pack(fill="x", padx=20, pady=10)

        advanced_label = ctk.CTkLabel(
            advanced_frame,
            text="🔧 Configuración Avanzada:",
            font=ctk.CTkFont(size=14, weight="bold")
        )
        advanced_label.pack(anchor="w", padx=10, pady=5)

        # Image quality slider
        quality_frame = ctk.CTkFrame(advanced_frame)
        quality_frame.pack(fill="x", padx=10, pady=5)

        quality_label = ctk.CTkLabel(quality_frame, text="Calidad de imagen:")
        quality_label.pack(anchor="w", padx=5, pady=2)

        self.quality_slider = ctk.CTkSlider(
            quality_frame,
            from_=1,
            to=100,
            number_of_steps=99,
            command=self.update_quality_label
        )
        self.quality_slider.set(85)
        self.quality_slider.pack(fill="x", padx=5, pady=2)

        self.quality_value_label = ctk.CTkLabel(
            quality_frame,
            text="85%",
            font=ctk.CTkFont(size=12)
        )
        self.quality_value_label.pack(anchor="w", padx=5)

        # Parallel processing toggle
        parallel_frame = ctk.CTkFrame(advanced_frame)
        parallel_frame.pack(fill="x", padx=10, pady=5)

        self.parallel_var = tk.BooleanVar(value=True)
        parallel_switch = ctk.CTkSwitch(
            parallel_frame,
            text="Procesamiento paralelo (más rápido)",
            variable=self.parallel_var,
            command=self.update_parallel_workers
        )
        parallel_switch.pack(anchor="w", padx=5, pady=5)

        # Supported formats info
        formats_frame = ctk.CTkFrame(options_frame)
        formats_frame.pack(fill="x", padx=20, pady=10)

        formats_label = ctk.CTkLabel(
            formats_frame,
            text="📋 Formatos Soportados:",
            font=ctk.CTkFont(size=14, weight="bold")
        )
        formats_label.pack(anchor="w", padx=10, pady=5)

        formats_text = ctk.CTkLabel(
            formats_frame,
            text="🖼️ Imágenes: PNG, JPG, JPEG, BMP, TIFF, WebP\n📄 Documentos: PDF\n📦 Otros: Todos los formatos (ZIP/GZIP)",
            font=ctk.CTkFont(size=12)
        )
        formats_text.pack(anchor="w", padx=10, pady=5)

    def create_progress_section(self, parent):
        """Create progress widgets with enhanced display"""
        # Progress frame
        self.progress_frame = ctk.CTkFrame(parent)
        self.progress_frame.pack(fill="x", padx=20, pady=10)

        # Section title
        section_title = ctk.CTkLabel(
            self.progress_frame,
            text="📊 Progreso",
            font=ctk.CTkFont(size=18, weight="bold")
        )
        section_title.pack(pady=(15, 10))

        # Progress bar
        self.progress_bar = ctk.CTkProgressBar(self.progress_frame)
        self.progress_bar.pack(fill="x", padx=20, pady=5)
        self.progress_bar.set(0)

        # Progress label
        self.progress_label = ctk.CTkLabel(
            self.progress_frame,
            text="Listo para comprimir",
            text_color="gray",
            font=ctk.CTkFont(size=12)
        )
        self.progress_label.pack(pady=5)

        # Time estimation
        self.time_label = ctk.CTkLabel(
            self.progress_frame,
            text="",
            text_color="gray",
            font=ctk.CTkFont(size=10)
        )
        self.time_label.pack(pady=2)

    def create_results_section(self, parent):
        """Create results display widgets with improved formatting"""
        # Results frame
        self.results_frame = ctk.CTkFrame(parent)
        self.results_frame.pack(fill="both", expand=True, padx=20, pady=10)

        # Section title
        section_title = ctk.CTkLabel(
            self.results_frame,
            text="📈 Resultados",
            font=ctk.CTkFont(size=18, weight="bold")
        )
        section_title.pack(pady=(15, 10))

        # Results text area with better styling
        self.results_text = ctk.CTkTextbox(
            self.results_frame,
            height=200,
            font=("Consolas", 11),
            wrap="word"
        )
        self.results_text.pack(fill="both", expand=True, padx=20, pady=10)

    def create_control_buttons(self, parent):
        """Create control buttons with improved styling"""
        # Buttons frame
        buttons_frame = ctk.CTkFrame(parent)
        buttons_frame.pack(fill="x", padx=20, pady=20)

        # Compress button
        self.compress_btn = ctk.CTkButton(
            buttons_frame,
            text="🚀 Comprimir Archivos",
            command=self.start_compression,
            height=45,
            font=ctk.CTkFont(size=16, weight="bold"),
            fg_color="#28a745",
            hover_color="#218838"
        )
        self.compress_btn.pack(side="left", padx=10, pady=10)

        # Stop button (initially disabled)
        self.stop_btn = ctk.CTkButton(
            buttons_frame,
            text="⏹️ Detener",
            command=self.stop_compression,
            height=45,
            font=ctk.CTkFont(size=16, weight="bold"),
            fg_color="#dc3545",
            hover_color="#c82333",
            state="disabled"
        )
        self.stop_btn.pack(side="left", padx=10, pady=10)

        # Exit button
        exit_btn = ctk.CTkButton(
            buttons_frame,
            text="❌ Salir",
            command=self.on_closing,
            height=45,
            font=ctk.CTkFont(size=16, weight="bold"),
            fg_color="#6c757d",
            hover_color="#5a6268"
        )
        exit_btn.pack(side="right", padx=10, pady=10)

    def create_status_bar(self):
        """Create status bar for additional information"""
        self.status_bar = ctk.CTkLabel(
            self.root,
            text="Listo",
            text_color="gray",
            font=ctk.CTkFont(size=10)
        )
        self.status_bar.pack(side="bottom", fill="x", padx=5, pady=2)



    def update_quality_label(self, value):
        """Update quality label when slider changes"""
        self.quality_value_label.configure(text=f"{int(value)}%")
        self.compression_config.image_quality = int(value)

    def update_parallel_workers(self):
        """Update parallel processing configuration"""
        if self.parallel_var.get():
            self.compression_config.max_workers = min(4, os.cpu_count() or 1)
        else:
            self.compression_config.max_workers = 1

    def add_files(self):
        """Add files to the list with improved file type detection"""
        files = filedialog.askopenfilenames(
            title="Seleccionar archivos para comprimir",
            filetypes=[
                ("Todos los archivos", "*.*"),
                ("Imágenes", "*.png *.jpg *.jpeg *.bmp *.tiff *.webp"),
                ("PDFs", "*.pdf"),
                ("Documentos", "*.txt *.doc *.docx *.xls *.xlsx"),
                ("Archivos comprimidos", "*.zip *.rar *.7z *.tar *.gz"),
            ]
        )

        if files:
            for file in files:
                if file not in self.selected_files:
                    self.selected_files.append(file)
                    self.file_listbox.insert(tk.END, os.path.basename(file))

            self.update_file_info()

    def add_folder(self):
        """Add all files from a folder"""
        folder = filedialog.askdirectory(title="Seleccionar carpeta")
        if folder:
            supported_extensions = {'.png', '.jpg', '.jpeg', '.bmp', '.tiff', '.webp', '.pdf', '.txt', '.doc', '.docx', '.xls', '.xlsx'}

            for root, dirs, files in os.walk(folder):
                for file in files:
                    file_path = os.path.join(root, file)
                    if Path(file).suffix.lower() in supported_extensions:
                        if file_path not in self.selected_files:
                            self.selected_files.append(file_path)
                            self.file_listbox.insert(tk.END, os.path.basename(file))

            self.update_file_info()

    def clear_files(self):
        """Clear the file list"""
        self.selected_files.clear()
        self.file_listbox.delete(0, tk.END)
        self.update_file_info()

    def update_file_info(self):
        """Update file information display"""
        if not self.selected_files:
            self.file_info_label.configure(text="No hay archivos seleccionados")
            return

        total_size = sum(os.path.getsize(f) for f in self.selected_files if os.path.exists(f))
        total_size_mb = total_size / (1024 * 1024)

        self.file_info_label.configure(
            text=f"📁 {len(self.selected_files)} archivo(s) seleccionado(s) - {total_size_mb:.2f} MB total"
        )

    def select_output_directory(self):
        """Select output directory with validation"""
        directory = filedialog.askdirectory(title="Seleccionar directorio de salida")
        if directory:
            # Check if directory is writable
            try:
                test_file = os.path.join(directory, ".test_write")
                with open(test_file, 'w') as f:
                    f.write("test")
                os.remove(test_file)

                self.output_directory = directory
                self.output_path_label.configure(
                    text=directory,
                    text_color="white"
                )
                self.status_bar.configure(text=f"Directorio de salida: {directory}")

            except PermissionError:
                messagebox.showerror("Error", "No tienes permisos de escritura en este directorio.")
            except Exception as e:
                messagebox.showerror("Error", f"Error al verificar el directorio: {str(e)}")

    def update_progress(self, message, percentage):
        """Update progress bar and label with time estimation"""
        if not self.is_compressing:
            return

        self.progress_bar.set(percentage / 100)
        self.progress_label.configure(text=message)

        # Calculate time estimation
        if self.start_time and percentage > 0:
            elapsed_time = time.time() - self.start_time
            estimated_total = elapsed_time / (percentage / 100)
            remaining_time = estimated_total - elapsed_time

            if remaining_time > 0:
                if remaining_time < 60:
                    time_text = f"Tiempo restante: {remaining_time:.1f}s"
                else:
                    time_text = f"Tiempo restante: {remaining_time/60:.1f}min"
                self.time_label.configure(text=time_text)

        self.root.update_idletasks()

    def start_compression(self):
        """Start the compression process with validation"""
        if not self.selected_files:
            messagebox.showwarning("Advertencia", "Por favor selecciona al menos un archivo.")
            return

        if not self.output_directory:
            messagebox.showwarning("Advertencia", "Por favor selecciona un directorio de salida.")
            return

        # Validate files exist
        valid_files = []
        for file_path in self.selected_files:
            if os.path.exists(file_path):
                valid_files.append(file_path)
            else:
                messagebox.showwarning("Advertencia", f"El archivo no existe: {os.path.basename(file_path)}")

        if not valid_files:
            messagebox.showerror("Error", "No hay archivos válidos para comprimir.")
            return

        # Update UI state
        self.is_compressing = True
        self.start_time = time.time()
        self.compress_btn.configure(state="disabled")
        self.stop_btn.configure(state="normal")

        # Clear results
        self.results_text.delete("1.0", tk.END)
        self.progress_bar.set(0)
        self.progress_label.configure(text="Iniciando compresión...")
        self.time_label.configure(text="")
        self.status_bar.configure(text="Comprimiendo archivos...")

        # Start compression in a separate thread
        thread = threading.Thread(target=self.compress_files, args=(valid_files,))
        thread.daemon = True
        thread.start()

    def stop_compression(self):
        """Stop the compression process"""
        self.is_compressing = False
        self.compress_btn.configure(state="normal")
        self.stop_btn.configure(state="disabled")
        self.progress_label.configure(text="Compresión detenida por el usuario")
        self.status_bar.configure(text="Compresión detenida")

    def compress_files(self, file_paths):
        """Compress files in background thread with enhanced error handling"""
        try:
            # Create compressor with current configuration
            compressor = FileCompressor(
                progress_callback=self.update_progress,
                config=self.compression_config
            )

            # Compress files
            results = compressor.compress_multiple_files(
                file_paths,
                self.output_directory,
                self.compression_type.get()
            )

            # Display results
            self.display_results(results)

            # Update UI state
            self.is_compressing = False
            self.compress_btn.configure(state="normal")
            self.stop_btn.configure(state="disabled")

            # Calculate total time
            if self.start_time:
                total_time = time.time() - self.start_time
                self.status_bar.configure(text=f"Compresión completada en {total_time:.1f}s")

            # Show completion message
            successful = sum(1 for r in results if r['success'])
            messagebox.showinfo(
                "Completado",
                f"Compresión completada!\n{successful}/{len(results)} archivos procesados exitosamente."
            )

        except Exception as e:
            messagebox.showerror("Error", f"Error durante la compresión: {str(e)}")
            self.is_compressing = False
            self.compress_btn.configure(state="normal")
            self.stop_btn.configure(state="disabled")
            self.status_bar.configure(text="Error en la compresión")

    def display_results(self, results):
        """Display compression results with enhanced formatting"""
        self.results_text.delete("1.0", tk.END)

        total_original = 0
        total_compressed = 0
        successful = 0

        # Add header
        self.results_text.insert(tk.END, "📊 RESULTADOS DE COMPRESIÓN\n")
        self.results_text.insert(tk.END, "=" * 50 + "\n\n")

        for result in results:
            if result['success']:
                successful += 1
                total_original += result['original_size']
                total_compressed += result['compressed_size']

                # Format sizes
                original_mb = result['original_size'] / (1024 * 1024)
                compressed_mb = result['compressed_size'] / (1024 * 1024)

                self.results_text.insert(tk.END, f"✅ {result['filename']}\n")
                self.results_text.insert(tk.END, f"   📏 Original: {original_mb:.2f} MB\n")
                self.results_text.insert(tk.END, f"   📦 Comprimido: {compressed_mb:.2f} MB\n")
                self.results_text.insert(tk.END, f"   📉 Compresión: {result['compression_ratio']:.1f}%\n")
                self.results_text.insert(tk.END, f"   📁 Guardado en: {result['output_path']}\n\n")
            else:
                self.results_text.insert(tk.END, f"❌ {result['filename']}: {result['error']}\n\n")

        # Summary
        if successful > 0:
            total_original_mb = total_original / (1024 * 1024)
            total_compressed_mb = total_compressed / (1024 * 1024)
            total_compression = (1 - total_compressed / total_original) * 100
            space_saved = total_original_mb - total_compressed_mb

            self.results_text.insert(tk.END, "📈 RESUMEN FINAL\n")
            self.results_text.insert(tk.END, "=" * 50 + "\n")
            self.results_text.insert(tk.END, f"📁 Archivos procesados: {successful}/{len(results)}\n")
            self.results_text.insert(tk.END, f"📏 Tamaño total original: {total_original_mb:.2f} MB\n")
            self.results_text.insert(tk.END, f"📦 Tamaño total comprimido: {total_compressed_mb:.2f} MB\n")
            self.results_text.insert(tk.END, f"💾 Espacio ahorrado: {space_saved:.2f} MB\n")
            self.results_text.insert(tk.END, f"📉 Compresión total: {total_compression:.1f}%\n")

            if self.start_time:
                total_time = time.time() - self.start_time
                self.results_text.insert(tk.END, f"⏱️ Tiempo total: {total_time:.1f} segundos\n")

    def on_closing(self):
        """Handle application closing"""
        if self.is_compressing:
            if messagebox.askokcancel("Salir", "La compresión está en progreso. ¿Deseas salir de todas formas?"):
                self.is_compressing = False
                self.root.quit()
        else:
            self.root.quit()

    def run(self):
        """Start the GUI application"""
        self.root.mainloop()

if __name__ == "__main__":
    app = CompressorGUI()
    app.run()
