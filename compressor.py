import os
import zipfile
import gzip
import shutil
from PIL import Image
import PyPDF2
from io import BytesIO
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import List, Dict, Optional, Callable, Union
import logging
from pathlib import Path
import mimetypes

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class CompressionConfig:
    """Configuration class for compression settings"""
    def __init__(self):
        self.image_quality = 85
        self.image_optimize = True
        self.image_progressive = True
        self.zip_compression_level = 9
        self.gzip_compression_level = 9
        self.max_workers = min(4, os.cpu_count() or 1)
        self.chunk_size = 8192  # 8KB chunks for file operations

class FileCompressor:
    def __init__(self, progress_callback: Optional[Callable] = None, config: Optional[CompressionConfig] = None):
        self.progress_callback = progress_callback
        self.config = config or CompressionConfig()
        self._supported_image_formats = {'.png', '.jpg', '.jpeg', '.bmp', '.tiff', '.webp'}
        self._supported_pdf_formats = {'.pdf'}

    def update_progress(self, message: str, percentage: float) -> None:
        """Update progress callback if provided"""
        if self.progress_callback:
            try:
                self.progress_callback(message, percentage)
            except Exception as e:
                logger.warning(f"Error in progress callback: {e}")

    def get_file_type(self, file_path: str) -> str:
        """Determine file type based on extension and mime type"""
        path = Path(file_path)
        extension = path.suffix.lower()

        if extension in self._supported_image_formats:
            return 'image'
        elif extension in self._supported_pdf_formats:
            return 'pdf'
        else:
            # Check mime type as fallback
            mime_type, _ = mimetypes.guess_type(file_path)
            if mime_type and mime_type.startswith('image/'):
                return 'image'
            elif mime_type == 'application/pdf':
                return 'pdf'
            else:
                return 'general'

    def compress_file(self, input_path: str, output_path: str, compression_type: str = "zip") -> Dict:
        """Compress a single file based on its type with improved error handling"""
        try:
            if not os.path.exists(input_path):
                raise FileNotFoundError(f"Archivo no encontrado: {input_path}")

            file_type = self.get_file_type(input_path)

            # Ensure output directory exists
            os.makedirs(os.path.dirname(output_path), exist_ok=True)

            if file_type == 'image':
                return self.compress_image(input_path, output_path)
            elif file_type == 'pdf':
                return self.compress_pdf(input_path, output_path)
            else:
                return self.compress_general_file(input_path, output_path, compression_type)

        except Exception as e:
            logger.error(f"Error comprimiendo {input_path}: {e}")
            raise Exception(f"Error comprimiendo {os.path.basename(input_path)}: {str(e)}")

    def compress_image(self, input_path: str, output_path: str) -> Dict:
        """Compress image files with optimized settings"""
        try:
            self.update_progress(f"Comprimiendo imagen: {os.path.basename(input_path)}", 10)

            # Open image with error handling
            with Image.open(input_path) as img:
                # Convert to RGB if necessary (better compression)
                if img.mode in ('RGBA', 'LA', 'P'):
                    # Create white background for transparent images
                    background = Image.new('RGB', img.size, (255, 255, 255))
                    if img.mode == 'P':
                        img = img.convert('RGBA')
                    background.paste(img, mask=img.split()[-1] if img.mode == 'RGBA' else None)
                    img = background
                elif img.mode != 'RGB':
                    img = img.convert('RGB')

                self.update_progress(f"Optimizando imagen...", 50)

                # Determine output format based on extension
                output_ext = Path(output_path).suffix.lower()

                # Optimize save parameters based on format
                save_kwargs = {
                    'optimize': self.config.image_optimize,
                    'quality': self.config.image_quality
                }

                if output_ext in ['.jpg', '.jpeg']:
                    save_kwargs['progressive'] = self.config.image_progressive
                elif output_ext == '.png':
                    # PNG optimization
                    save_kwargs.pop('quality', None)  # PNG doesn't use quality
                    save_kwargs['optimize'] = True
                    save_kwargs['compress_level'] = 9

                # Save with optimization
                img.save(output_path, **save_kwargs)

                self.update_progress(f"Imagen comprimida exitosamente", 100)

                return self._calculate_compression_stats(input_path, output_path)

        except Exception as e:
            logger.error(f"Error comprimiendo imagen {input_path}: {e}")
            raise Exception(f"Error comprimiendo imagen: {str(e)}")

    def compress_pdf(self, input_path: str, output_path: str) -> Dict:
        """Compress PDF files with improved error handling"""
        try:
            self.update_progress(f"Comprimiendo PDF: {os.path.basename(input_path)}", 10)

            # Read PDF with error handling
            with open(input_path, 'rb') as file:
                reader = PyPDF2.PdfReader(file)

                if not reader.pages:
                    raise ValueError("PDF no contiene páginas válidas")

                writer = PyPDF2.PdfWriter()

                self.update_progress(f"Procesando {len(reader.pages)} páginas...", 30)

                # Add pages to writer with progress
                for i, page in enumerate(reader.pages):
                    writer.add_page(page)
                    progress = 30 + (i / len(reader.pages)) * 40
                    self.update_progress(f"Procesando página {i+1}/{len(reader.pages)}", progress)

                self.update_progress(f"Optimizando PDF...", 70)

                # Write compressed PDF
                with open(output_path, 'wb') as output_file:
                    writer.write(output_file)

                self.update_progress(f"PDF comprimido exitosamente", 100)

                return self._calculate_compression_stats(input_path, output_path)

        except Exception as e:
            logger.error(f"Error comprimiendo PDF {input_path}: {e}")
            raise Exception(f"Error comprimiendo PDF: {str(e)}")

    def compress_general_file(self, input_path: str, output_path: str, compression_type: str = "zip") -> Dict:
        """Compress general files using ZIP or GZIP with optimized settings"""
        try:
            self.update_progress(f"Comprimiendo archivo: {os.path.basename(input_path)}", 10)

            if compression_type == "zip":
                with zipfile.ZipFile(
                    output_path,
                    'w',
                    zipfile.ZIP_DEFLATED,
                    compresslevel=self.config.zip_compression_level
                ) as zipf:
                    zipf.write(input_path, os.path.basename(input_path))

            elif compression_type == "gzip":
                with open(input_path, 'rb') as f_in:
                    with gzip.open(
                        output_path,
                        'wb',
                        compresslevel=self.config.gzip_compression_level
                    ) as f_out:
                        shutil.copyfileobj(f_in, f_out, length=self.config.chunk_size)

            self.update_progress(f"Archivo comprimido exitosamente", 100)

            return self._calculate_compression_stats(input_path, output_path)

        except Exception as e:
            logger.error(f"Error comprimiendo archivo {input_path}: {e}")
            raise Exception(f"Error comprimiendo archivo: {str(e)}")

    def _calculate_compression_stats(self, input_path: str, output_path: str) -> Dict:
        """Calculate compression statistics"""
        original_size = os.path.getsize(input_path)
        compressed_size = os.path.getsize(output_path)
        compression_ratio = (1 - compressed_size / original_size) * 100 if original_size > 0 else 0

        return {
            'success': True,
            'original_size': original_size,
            'compressed_size': compressed_size,
            'compression_ratio': compression_ratio
        }

    def _compress_single_file_worker(self, args: tuple) -> Dict:
        """Worker function for parallel compression"""
        file_path, output_dir, compression_type = args
        try:
            filename = os.path.basename(file_path)
            name, ext = os.path.splitext(filename)
            file_type = self.get_file_type(file_path)

            if file_type == 'image':
                output_path = os.path.join(output_dir, f"{name}_compressed{ext}")
            elif file_type == 'pdf':
                output_path = os.path.join(output_dir, f"{name}_compressed{ext}")
            else:
                if compression_type == "zip":
                    output_path = os.path.join(output_dir, f"{name}.zip")
                else:
                    output_path = os.path.join(output_dir, f"{name}.gz")

            result = self.compress_file(file_path, output_path, compression_type)
            result['filename'] = filename
            result['output_path'] = output_path
            return result

        except Exception as e:
            return {
                'success': False,
                'filename': os.path.basename(file_path),
                'error': str(e)
            }

    def compress_multiple_files(self, file_paths: List[str], output_dir: str, compression_type: str = "zip") -> List[Dict]:
        """Compress multiple files with parallel processing"""
        if not file_paths:
            return []

        # Ensure output directory exists
        os.makedirs(output_dir, exist_ok=True)

        results = []
        total_files = len(file_paths)

        # Use parallel processing for multiple files
        if total_files > 1 and self.config.max_workers > 1:
            self.update_progress(f"Iniciando compresión paralela de {total_files} archivos...", 0)

            with ThreadPoolExecutor(max_workers=self.config.max_workers) as executor:
                # Prepare arguments for workers
                args_list = [(file_path, output_dir, compression_type) for file_path in file_paths]

                # Submit all tasks
                future_to_file = {executor.submit(self._compress_single_file_worker, args): args[0]
                                for args in args_list}

                # Collect results as they complete
                completed = 0
                for future in as_completed(future_to_file):
                    result = future.result()
                    results.append(result)
                    completed += 1

                    # Update progress
                    progress = (completed / total_files) * 100
                    self.update_progress(f"Progreso: {completed}/{total_files} archivos", progress)
        else:
            # Sequential processing for single file or when parallel processing is disabled
            for i, file_path in enumerate(file_paths):
                try:
                    result = self._compress_single_file_worker((file_path, output_dir, compression_type))
                    results.append(result)

                    # Update progress
                    progress = ((i + 1) / total_files) * 100
                    self.update_progress(f"Progreso: {i + 1}/{total_files} archivos", progress)

                except Exception as e:
                    results.append({
                        'success': False,
                        'filename': os.path.basename(file_path),
                        'error': str(e)
                    })

        return results

    def get_compression_info(self, file_path: str) -> Dict:
        """Get information about file compression potential"""
        try:
            file_type = self.get_file_type(file_path)
            file_size = os.path.getsize(file_path)

            info = {
                'file_type': file_type,
                'file_size': file_size,
                'file_size_mb': file_size / (1024 * 1024),
                'extension': Path(file_path).suffix.lower(),
                'filename': os.path.basename(file_path)
            }

            # Add type-specific information
            if file_type == 'image':
                with Image.open(file_path) as img:
                    info.update({
                        'dimensions': img.size,
                        'mode': img.mode,
                        'format': img.format
                    })
            elif file_type == 'pdf':
                with open(file_path, 'rb') as file:
                    reader = PyPDF2.PdfReader(file)
                    info.update({
                        'pages': len(reader.pages)
                    })

            return info

        except Exception as e:
            logger.error(f"Error getting compression info for {file_path}: {e}")
            return {
                'error': str(e),
                'filename': os.path.basename(file_path)
            }
