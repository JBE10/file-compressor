#!/usr/bin/env python3
"""
Optimizador de Compresión Avanzado
==================================

Script adicional para optimización avanzada de compresión con diferentes
algoritmos y configuraciones personalizadas.

Características:
- Múltiples algoritmos de compresión
- Análisis de eficiencia de compresión
- Configuraciones personalizadas
- Comparación de resultados
"""

import os
import time
import json
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from concurrent.futures import ThreadPoolExecutor, as_completed
import logging
from compressor import FileCompressor, CompressionConfig

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class AdvancedCompressionOptimizer:
    """Advanced compression optimizer with multiple algorithms"""

    def __init__(self):
        self.results_cache = {}
        self.optimization_history = []

    def create_optimization_configs(self) -> List[CompressionConfig]:
        """Create different compression configurations for testing"""
        configs = []

        # High quality config
        high_quality = CompressionConfig()
        high_quality.image_quality = 95
        high_quality.image_optimize = True
        high_quality.image_progressive = True
        high_quality.zip_compression_level = 9
        high_quality.gzip_compression_level = 9
        high_quality.max_workers = 1
        configs.append(("Alta Calidad", high_quality))

        # Balanced config
        balanced = CompressionConfig()
        balanced.image_quality = 85
        balanced.image_optimize = True
        balanced.image_progressive = True
        balanced.zip_compression_level = 6
        balanced.gzip_compression_level = 6
        balanced.max_workers = 2
        configs.append(("Equilibrado", balanced))

        # High compression config
        high_compression = CompressionConfig()
        high_compression.image_quality = 70
        high_compression.image_optimize = True
        high_compression.image_progressive = False
        high_compression.zip_compression_level = 9
        high_compression.gzip_compression_level = 9
        high_compression.max_workers = 4
        configs.append(("Alta Compresión", high_compression))

        # Fast config
        fast = CompressionConfig()
        fast.image_quality = 60
        fast.image_optimize = False
        fast.image_progressive = False
        fast.zip_compression_level = 1
        fast.gzip_compression_level = 1
        fast.max_workers = 4
        configs.append(("Rápido", fast))

        return configs

    def analyze_file_compression_potential(self, file_path: str) -> Dict:
        """Analyze file compression potential"""
        try:
            compressor = FileCompressor()
            info = compressor.get_compression_info(file_path)

            # Add compression potential analysis
            file_size = info.get('file_size', 0)
            file_type = info.get('file_type', 'unknown')

            # Estimate compression potential based on file type
            if file_type == 'image':
                # Images typically compress well
                potential = 0.3  # 30% average compression
            elif file_type == 'pdf':
                # PDFs have variable compression potential
                potential = 0.2  # 20% average compression
            else:
                # General files depend on content
                potential = 0.1  # 10% average compression

            info['compression_potential'] = potential
            info['estimated_compressed_size'] = file_size * (1 - potential)

            return info

        except Exception as e:
            logger.error(f"Error analyzing file {file_path}: {e}")
            return {'error': str(e), 'filename': os.path.basename(file_path)}

    def test_compression_config(self, file_path: str, config_name: str,
                              config: CompressionConfig, output_dir: str) -> Dict:
        """Test a specific compression configuration"""
        try:
            start_time = time.time()

            # Create compressor with specific config
            compressor = FileCompressor(config=config)

            # Generate output path
            filename = os.path.basename(file_path)
            name, ext = os.path.splitext(filename)
            output_path = os.path.join(output_dir, f"{name}_{config_name.lower().replace(' ', '_')}{ext}")

            # Compress file
            result = compressor.compress_file(file_path, output_path, "zip")

            # Calculate performance metrics
            compression_time = time.time() - start_time
            compression_speed = result['original_size'] / compression_time if compression_time > 0 else 0

            # Add performance metrics to result
            result.update({
                'config_name': config_name,
                'compression_time': compression_time,
                'compression_speed_mbps': compression_speed / (1024 * 1024),  # MB/s
                'output_path': output_path
            })

            return result

        except Exception as e:
            logger.error(f"Error testing config {config_name} on {file_path}: {e}")
            return {
                'success': False,
                'config_name': config_name,
                'error': str(e),
                'filename': os.path.basename(file_path)
            }

    def find_optimal_config(self, file_paths: List[str], output_dir: str) -> Dict:
        """Find the optimal compression configuration for given files"""
        try:
            print("🔍 Analizando archivos para encontrar la configuración óptima...")

            # Create output directory
            os.makedirs(output_dir, exist_ok=True)

            # Get all configurations
            configs = self.create_optimization_configs()

            # Analyze files
            file_analyses = []
            for file_path in file_paths:
                analysis = self.analyze_file_compression_potential(file_path)
                file_analyses.append(analysis)
                print(f"📊 {os.path.basename(file_path)}: {analysis.get('file_type', 'unknown')} - {analysis.get('file_size_mb', 0):.2f} MB")

            # Test each configuration
            all_results = []

            for config_name, config in configs:
                print(f"\n⚙️ Probando configuración: {config_name}")

                config_results = []
                for file_path in file_paths:
                    result = self.test_compression_config(file_path, config_name, config, output_dir)
                    config_results.append(result)

                # Calculate aggregate metrics for this config
                successful_results = [r for r in config_results if r['success']]
                if successful_results:
                    total_original = sum(r['original_size'] for r in successful_results)
                    total_compressed = sum(r['compressed_size'] for r in successful_results)
                    total_time = sum(r['compression_time'] for r in successful_results)
                    avg_speed = sum(r['compression_speed_mbps'] for r in successful_results) / len(successful_results)

                    config_summary = {
                        'config_name': config_name,
                        'total_original_mb': total_original / (1024 * 1024),
                        'total_compressed_mb': total_compressed / (1024 * 1024),
                        'total_compression_ratio': (1 - total_compressed / total_original) * 100,
                        'total_time': total_time,
                        'avg_speed_mbps': avg_speed,
                        'successful_files': len(successful_results),
                        'total_files': len(file_paths)
                    }

                    all_results.append(config_summary)

                    print(f"   ✅ Compresión: {config_summary['total_compression_ratio']:.1f}%")
                    print(f"   ⏱️ Tiempo: {config_summary['total_time']:.2f}s")
                    print(f"   🚀 Velocidad: {config_summary['avg_speed_mbps']:.2f} MB/s")

            # Find optimal configuration
            if all_results:
                # Sort by compression ratio (descending) and speed (descending)
                optimal = max(all_results, key=lambda x: (x['total_compression_ratio'], x['avg_speed_mbps']))

                print(f"\n🏆 Configuración óptima encontrada: {optimal['config_name']}")
                print(f"   📉 Compresión: {optimal['total_compression_ratio']:.1f}%")
                print(f"   ⏱️ Tiempo: {optimal['total_time']:.2f}s")
                print(f"   🚀 Velocidad: {optimal['avg_speed_mbps']:.2f} MB/s")

                return {
                    'optimal_config': optimal,
                    'all_results': all_results,
                    'file_analyses': file_analyses
                }
            else:
                print("❌ No se pudieron procesar archivos con ninguna configuración")
                return {'error': 'No successful compressions'}

        except Exception as e:
            logger.error(f"Error finding optimal config: {e}")
            return {'error': str(e)}

    def save_optimization_report(self, results: Dict, output_path: str):
        """Save optimization results to a JSON report"""
        try:
            with open(output_path, 'w', encoding='utf-8') as f:
                json.dump(results, f, indent=2, ensure_ascii=False)
            print(f"📄 Reporte guardado en: {output_path}")
        except Exception as e:
            logger.error(f"Error saving report: {e}")

def main():
    """Main function for the optimization script"""
    print("🚀 Optimizador de Compresión Avanzado")
    print("=" * 50)

    # Example usage
    optimizer = AdvancedCompressionOptimizer()

    # Get files to optimize (you can modify this part)
    test_files = []

    # Check if files were provided as arguments
    if len(sys.argv) > 1:
        test_files = sys.argv[1:]
    else:
        # Use example files if available
        current_dir = Path(__file__).parent
        for ext in ['.jpg', '.png', '.pdf', '.txt']:
            files = list(current_dir.glob(f"*{ext}"))
            test_files.extend([str(f) for f in files[:2]])  # Limit to 2 files per type

    if not test_files:
        print("❌ No se encontraron archivos para optimizar")
        print("Uso: python optimize_compression.py archivo1 archivo2 ...")
        return

    print(f"📁 Archivos a optimizar: {len(test_files)}")
    for file in test_files:
        print(f"   • {os.path.basename(file)}")

    # Create output directory
    output_dir = Path(__file__).parent / "optimization_results"
    output_dir.mkdir(exist_ok=True)

    # Find optimal configuration
    results = optimizer.find_optimal_config(test_files, str(output_dir))

    if 'error' not in results:
        # Save report
        report_path = output_dir / "optimization_report.json"
        optimizer.save_optimization_report(results, str(report_path))

        print("\n✅ Optimización completada exitosamente!")
    else:
        print(f"\n❌ Error en la optimización: {results['error']}")

if __name__ == "__main__":
    import sys
    main()
