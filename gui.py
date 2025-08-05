import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import customtkinter as ctk
import os
import threading
from compressor import FileCompressor

class CompressorGUI:
    def __init__(self):
        # Configure CustomTkinter
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")

        # Create main window
        self.root = ctk.CTk()
        self.root.title("Compresor de Archivos")
        self.root.geometry("800x600")
        self.root.resizable(True, True)

        # Variables
        self.selected_files = []
        self.output_directory = ""
        self.compression_type = tk.StringVar(value="zip")

        # Create GUI elements
        self.create_widgets()

    def create_widgets(self):
        """Create all GUI widgets"""
        # Main frame
        main_frame = ctk.CTkFrame(self.root)
        main_frame.pack(fill="both", expand=True, padx=20, pady=20)

        # Title
        title_label = ctk.CTkLabel(
            main_frame,
            text="Compresor de Archivos",
            font=ctk.CTkFont(size=24, weight="bold")
        )
        title_label.pack(pady=(20, 30))

        # File selection section
        self.create_file_selection_section(main_frame)

        # Compression options section
        self.create_compression_options_section(main_frame)

        # Progress section
        self.create_progress_section(main_frame)

        # Results section
        self.create_results_section(main_frame)

        # Control buttons
        self.create_control_buttons(main_frame)

    def create_file_selection_section(self, parent):
        """Create file selection widgets"""
        # File selection frame
        file_frame = ctk.CTkFrame(parent)
        file_frame.pack(fill="x", padx=20, pady=10)

        # Section title
        section_title = ctk.CTkLabel(
            file_frame,
            text="Selección de Archivos",
            font=ctk.CTkFont(size=16, weight="bold")
        )
        section_title.pack(pady=(15, 10))

        # File list
        self.file_listbox = tk.Listbox(
            file_frame,
            height=6,
            bg="#2b2b2b",
            fg="white",
            selectmode=tk.EXTENDED,
            font=("Arial", 10)
        )
        self.file_listbox.pack(fill="x", padx=20, pady=5)

        # File buttons frame
        file_buttons_frame = ctk.CTkFrame(file_frame)
        file_buttons_frame.pack(fill="x", padx=20, pady=10)

        # Add files button
        add_files_btn = ctk.CTkButton(
            file_buttons_frame,
            text="Agregar Archivos",
            command=self.add_files,
            width=120
        )
        add_files_btn.pack(side="left", padx=5)

        # Clear files button
        clear_files_btn = ctk.CTkButton(
            file_buttons_frame,
            text="Limpiar Lista",
            command=self.clear_files,
            width=120
        )
        clear_files_btn.pack(side="left", padx=5)

        # Output directory selection
        output_frame = ctk.CTkFrame(file_frame)
        output_frame.pack(fill="x", padx=20, pady=10)

        output_label = ctk.CTkLabel(output_frame, text="Directorio de Salida:")
        output_label.pack(anchor="w", padx=10, pady=5)

        self.output_path_label = ctk.CTkLabel(
            output_frame,
            text="No seleccionado",
            text_color="gray"
        )
        self.output_path_label.pack(anchor="w", padx=10, pady=2)

        select_output_btn = ctk.CTkButton(
            output_frame,
            text="Seleccionar Directorio",
            command=self.select_output_directory,
            width=150
        )
        select_output_btn.pack(pady=10)

    def create_compression_options_section(self, parent):
        """Create compression options widgets"""
        # Options frame
        options_frame = ctk.CTkFrame(parent)
        options_frame.pack(fill="x", padx=20, pady=10)

        # Section title
        section_title = ctk.CTkLabel(
            options_frame,
            text="Opciones de Compresión",
            font=ctk.CTkFont(size=16, weight="bold")
        )
        section_title.pack(pady=(15, 10))

        # Compression type selection
        compression_frame = ctk.CTkFrame(options_frame)
        compression_frame.pack(fill="x", padx=20, pady=10)

        compression_label = ctk.CTkLabel(compression_frame, text="Tipo de Compresión:")
        compression_label.pack(anchor="w", padx=10, pady=5)

        # Radio buttons for compression type
        zip_radio = ctk.CTkRadioButton(
            compression_frame,
            text="ZIP (Recomendado)",
            variable=self.compression_type,
            value="zip"
        )
        zip_radio.pack(anchor="w", padx=10, pady=2)

        gzip_radio = ctk.CTkRadioButton(
            compression_frame,
            text="GZIP",
            variable=self.compression_type,
            value="gzip"
        )
        gzip_radio.pack(anchor="w", padx=10, pady=2)

        # Supported formats info
        formats_frame = ctk.CTkFrame(options_frame)
        formats_frame.pack(fill="x", padx=20, pady=10)

        formats_label = ctk.CTkLabel(
            formats_frame,
            text="Formatos Soportados:",
            font=ctk.CTkFont(size=12, weight="bold")
        )
        formats_label.pack(anchor="w", padx=10, pady=5)

        formats_text = ctk.CTkLabel(
            formats_frame,
            text="• Imágenes: PNG, JPG, JPEG, BMP, TIFF\n• Documentos: PDF\n• Otros: Todos los formatos (ZIP/GZIP)"
        )
        formats_text.pack(anchor="w", padx=10, pady=5)

    def create_progress_section(self, parent):
        """Create progress widgets"""
        # Progress frame
        self.progress_frame = ctk.CTkFrame(parent)
        self.progress_frame.pack(fill="x", padx=20, pady=10)

        # Section title
        section_title = ctk.CTkLabel(
            self.progress_frame,
            text="Progreso",
            font=ctk.CTkFont(size=16, weight="bold")
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
            text_color="gray"
        )
        self.progress_label.pack(pady=5)

    def create_results_section(self, parent):
        """Create results display widgets"""
        # Results frame
        self.results_frame = ctk.CTkFrame(parent)
        self.results_frame.pack(fill="both", expand=True, padx=20, pady=10)

        # Section title
        section_title = ctk.CTkLabel(
            self.results_frame,
            text="Resultados",
            font=ctk.CTkFont(size=16, weight="bold")
        )
        section_title.pack(pady=(15, 10))

        # Results text area
        self.results_text = ctk.CTkTextbox(
            self.results_frame,
            height=150,
            font=("Consolas", 10)
        )
        self.results_text.pack(fill="both", expand=True, padx=20, pady=10)

    def create_control_buttons(self, parent):
        """Create control buttons"""
        # Buttons frame
        buttons_frame = ctk.CTkFrame(parent)
        buttons_frame.pack(fill="x", padx=20, pady=20)

        # Compress button
        self.compress_btn = ctk.CTkButton(
            buttons_frame,
            text="Comprimir Archivos",
            command=self.start_compression,
            height=40,
            font=ctk.CTkFont(size=14, weight="bold")
        )
        self.compress_btn.pack(side="left", padx=10, pady=10)

        # Exit button
        exit_btn = ctk.CTkButton(
            buttons_frame,
            text="Salir",
            command=self.root.quit,
            height=40,
            fg_color="red",
            hover_color="darkred"
        )
        exit_btn.pack(side="right", padx=10, pady=10)

    def add_files(self):
        """Add files to the list"""
        files = filedialog.askopenfilenames(
            title="Seleccionar archivos para comprimir",
            filetypes=[
                ("Todos los archivos", "*.*"),
                ("Imágenes", "*.png *.jpg *.jpeg *.bmp *.tiff"),
                ("PDFs", "*.pdf"),
                ("Archivos de texto", "*.txt *.doc *.docx"),
            ]
        )

        for file in files:
            if file not in self.selected_files:
                self.selected_files.append(file)
                self.file_listbox.insert(tk.END, os.path.basename(file))

    def clear_files(self):
        """Clear the file list"""
        self.selected_files.clear()
        self.file_listbox.delete(0, tk.END)

    def select_output_directory(self):
        """Select output directory"""
        directory = filedialog.askdirectory(title="Seleccionar directorio de salida")
        if directory:
            self.output_directory = directory
            self.output_path_label.configure(
                text=directory,
                text_color="white"
            )

    def update_progress(self, message, percentage):
        """Update progress bar and label"""
        self.progress_bar.set(percentage / 100)
        self.progress_label.configure(text=message)
        self.root.update_idletasks()

    def start_compression(self):
        """Start the compression process"""
        if not self.selected_files:
            messagebox.showwarning("Advertencia", "Por favor selecciona al menos un archivo.")
            return

        if not self.output_directory:
            messagebox.showwarning("Advertencia", "Por favor selecciona un directorio de salida.")
            return

        # Disable compress button
        self.compress_btn.configure(state="disabled")

        # Clear results
        self.results_text.delete("1.0", tk.END)

        # Start compression in a separate thread
        thread = threading.Thread(target=self.compress_files)
        thread.daemon = True
        thread.start()

    def compress_files(self):
        """Compress files in background thread"""
        try:
            # Create compressor
            compressor = FileCompressor(progress_callback=self.update_progress)

            # Compress files
            results = compressor.compress_multiple_files(
                self.selected_files,
                self.output_directory,
                self.compression_type.get()
            )

            # Display results
            self.display_results(results)

            # Re-enable compress button
            self.compress_btn.configure(state="normal")

            # Show completion message
            messagebox.showinfo("Completado", "Compresión completada exitosamente!")

        except Exception as e:
            messagebox.showerror("Error", f"Error durante la compresión: {str(e)}")
            self.compress_btn.configure(state="normal")

    def display_results(self, results):
        """Display compression results"""
        self.results_text.delete("1.0", tk.END)

        total_original = 0
        total_compressed = 0
        successful = 0

        for result in results:
            if result['success']:
                successful += 1
                total_original += result['original_size']
                total_compressed += result['compressed_size']

                # Format sizes
                original_mb = result['original_size'] / (1024 * 1024)
                compressed_mb = result['compressed_size'] / (1024 * 1024)

                self.results_text.insert(tk.END, f"✓ {result['filename']}\n")
                self.results_text.insert(tk.END, f"   Original: {original_mb:.2f} MB\n")
                self.results_text.insert(tk.END, f"   Comprimido: {compressed_mb:.2f} MB\n")
                self.results_text.insert(tk.END, f"   Compresión: {result['compression_ratio']:.1f}%\n\n")
            else:
                self.results_text.insert(tk.END, f"✗ {result['filename']}: {result['error']}\n\n")

        # Summary
        if successful > 0:
            total_original_mb = total_original / (1024 * 1024)
            total_compressed_mb = total_compressed / (1024 * 1024)
            total_compression = (1 - total_compressed / total_original) * 100

            self.results_text.insert(tk.END, "=" * 50 + "\n")
            self.results_text.insert(tk.END, f"RESUMEN:\n")
            self.results_text.insert(tk.END, f"Archivos procesados: {successful}/{len(results)}\n")
            self.results_text.insert(tk.END, f"Tamaño total original: {total_original_mb:.2f} MB\n")
            self.results_text.insert(tk.END, f"Tamaño total comprimido: {total_compressed_mb:.2f} MB\n")
            self.results_text.insert(tk.END, f"Compresión total: {total_compression:.1f}%\n")

    def run(self):
        """Start the GUI application"""
        self.root.mainloop()

if __name__ == "__main__":
    app = CompressorGUI()
    app.run()
