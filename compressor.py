import os
import zipfile
import gzip
import shutil
from PIL import Image
import PyPDF2
from io import BytesIO
import threading
import time

class FileCompressor:
    def __init__(self, progress_callback=None):
        self.progress_callback = progress_callback

    def update_progress(self, message, percentage):
        """Update progress callback if provided"""
        if self.progress_callback:
            self.progress_callback(message, percentage)

    def compress_file(self, input_path, output_path, compression_type="zip"):
        """Compress a single file based on its type"""
        try:
            file_extension = os.path.splitext(input_path)[1].lower()

            if file_extension in ['.png', '.jpg', '.jpeg', '.bmp', '.tiff']:
                return self.compress_image(input_path, output_path)
            elif file_extension == '.pdf':
                return self.compress_pdf(input_path, output_path)
            else:
                return self.compress_general_file(input_path, output_path, compression_type)

        except Exception as e:
            raise Exception(f"Error comprimiendo {input_path}: {str(e)}")

    def compress_image(self, input_path, output_path):
        """Compress image files"""
        try:
            self.update_progress(f"Comprimiendo imagen: {os.path.basename(input_path)}", 10)

            # Open image
            with Image.open(input_path) as img:
                # Convert to RGB if necessary
                if img.mode in ('RGBA', 'LA', 'P'):
                    img = img.convert('RGB')

                self.update_progress(f"Optimizando imagen...", 50)

                # Save with optimization
                img.save(output_path,
                        optimize=True,
                        quality=85,  # Good balance between quality and size
                        progressive=True)

                self.update_progress(f"Imagen comprimida exitosamente", 100)

                # Calculate compression ratio
                original_size = os.path.getsize(input_path)
                compressed_size = os.path.getsize(output_path)
                compression_ratio = (1 - compressed_size / original_size) * 100

                return {
                    'success': True,
                    'original_size': original_size,
                    'compressed_size': compressed_size,
                    'compression_ratio': compression_ratio
                }

        except Exception as e:
            raise Exception(f"Error comprimiendo imagen: {str(e)}")

    def compress_pdf(self, input_path, output_path):
        """Compress PDF files"""
        try:
            self.update_progress(f"Comprimiendo PDF: {os.path.basename(input_path)}", 10)

            # Read PDF
            with open(input_path, 'rb') as file:
                reader = PyPDF2.PdfReader(file)
                writer = PyPDF2.PdfWriter()

                self.update_progress(f"Procesando páginas del PDF...", 30)

                # Add pages to writer
                for page in reader.pages:
                    writer.add_page(page)

                self.update_progress(f"Optimizando PDF...", 70)

                # Write compressed PDF
                with open(output_path, 'wb') as output_file:
                    writer.write(output_file)

                self.update_progress(f"PDF comprimido exitosamente", 100)

                # Calculate compression ratio
                original_size = os.path.getsize(input_path)
                compressed_size = os.path.getsize(output_path)
                compression_ratio = (1 - compressed_size / original_size) * 100

                return {
                    'success': True,
                    'original_size': original_size,
                    'compressed_size': compressed_size,
                    'compression_ratio': compression_ratio
                }

        except Exception as e:
            raise Exception(f"Error comprimiendo PDF: {str(e)}")

    def compress_general_file(self, input_path, output_path, compression_type="zip"):
        """Compress general files using ZIP or GZIP"""
        try:
            self.update_progress(f"Comprimiendo archivo: {os.path.basename(input_path)}", 10)

            if compression_type == "zip":
                with zipfile.ZipFile(output_path, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as zipf:
                    zipf.write(input_path, os.path.basename(input_path))
            elif compression_type == "gzip":
                with open(input_path, 'rb') as f_in:
                    with gzip.open(output_path, 'wb', compresslevel=9) as f_out:
                        shutil.copyfileobj(f_in, f_out)

            self.update_progress(f"Archivo comprimido exitosamente", 100)

            # Calculate compression ratio
            original_size = os.path.getsize(input_path)
            compressed_size = os.path.getsize(output_path)
            compression_ratio = (1 - compressed_size / original_size) * 100

            return {
                'success': True,
                'original_size': original_size,
                'compressed_size': compressed_size,
                'compression_ratio': compression_ratio
            }

        except Exception as e:
            raise Exception(f"Error comprimiendo archivo: {str(e)}")

    def compress_multiple_files(self, file_paths, output_dir, compression_type="zip"):
        """Compress multiple files"""
        results = []
        total_files = len(file_paths)

        for i, file_path in enumerate(file_paths):
            try:
                # Create output filename
                filename = os.path.basename(file_path)
                name, ext = os.path.splitext(filename)

                if ext.lower() in ['.png', '.jpg', '.jpeg', '.bmp', '.tiff']:
                    output_path = os.path.join(output_dir, f"{name}_compressed{ext}")
                elif ext.lower() == '.pdf':
                    output_path = os.path.join(output_dir, f"{name}_compressed{ext}")
                else:
                    if compression_type == "zip":
                        output_path = os.path.join(output_dir, f"{name}.zip")
                    else:
                        output_path = os.path.join(output_dir, f"{name}.gz")

                # Compress file
                result = self.compress_file(file_path, output_path, compression_type)
                result['filename'] = filename
                result['output_path'] = output_path
                results.append(result)

                # Update overall progress
                progress = ((i + 1) / total_files) * 100
                self.update_progress(f"Progreso: {i + 1}/{total_files} archivos", progress)

            except Exception as e:
                results.append({
                    'success': False,
                    'filename': os.path.basename(file_path),
                    'error': str(e)
                })

        return results
