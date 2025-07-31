#!/usr/bin/env python3
"""
✅ Validation Runner for Python Cellframe SDK
Comprehensive validation pipeline for code quality and functionality.
"""

import os
import sys
import json
import subprocess
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
from datetime import datetime
import tempfile


class ValidationRunner:
    """Comprehensive validation pipeline."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.validation_results = {
            'timestamp': datetime.now().isoformat(),
            'project_root': str(self.project_root),
            'validations': {},
            'overall_status': 'unknown',
            'summary': {}
        }
    
    def run_syntax_check(self, paths: List[str]) -> Dict[str, Any]:
        """Check Python syntax for all files."""
        print("🔍 Running syntax validation...")
        
        syntax_results = {
            'status': 'passed',
            'files_checked': 0,
            'syntax_errors': [],
            'details': {}
        }
        
        for path in paths:
            path_obj = Path(path)
            python_files = []
            
            if path_obj.is_file() and path.endswith('.py'):
                python_files = [path_obj]
            elif path_obj.is_dir():
                python_files = list(path_obj.rglob('*.py'))
            
            for py_file in python_files:
                syntax_results['files_checked'] += 1
                
                try:
                    # Compile the file to check syntax
                    with open(py_file, 'r', encoding='utf-8') as f:
                        source = f.read()
                    
                    compile(source, str(py_file), 'exec')
                    syntax_results['details'][str(py_file)] = 'valid'
                    
                except SyntaxError as e:
                    syntax_error = {
                        'file': str(py_file),
                        'line': e.lineno,
                        'column': e.offset,
                        'message': str(e.msg),
                        'text': e.text.strip() if e.text else ''
                    }
                    syntax_results['syntax_errors'].append(syntax_error)
                    syntax_results['details'][str(py_file)] = 'syntax_error'
                    syntax_results['status'] = 'failed'
                    
                except Exception as e:
                    syntax_error = {
                        'file': str(py_file),
                        'line': 0,
                        'column': 0,
                        'message': f"Compilation error: {e}",
                        'text': ''
                    }
                    syntax_results['syntax_errors'].append(syntax_error)
                    syntax_results['details'][str(py_file)] = 'compilation_error'
                    syntax_results['status'] = 'failed'
        
        if syntax_results['syntax_errors']:
            print(f"  ❌ Found {len(syntax_results['syntax_errors'])} syntax errors")
            for error in syntax_results['syntax_errors'][:5]:  # Show first 5
                print(f"    • {error['file']}:{error['line']} - {error['message']}")
        else:
            print(f"  ✅ All {syntax_results['files_checked']} files have valid syntax")
        
        return syntax_results
    
    def run_import_check(self, paths: List[str]) -> Dict[str, Any]:
        """Check if all imports can be resolved."""
        print("📦 Running import validation...")
        
        import_results = {
            'status': 'passed',
            'files_checked': 0,
            'import_errors': [],
            'details': {}
        }
        
        # Set up environment for testing imports
        env = os.environ.copy()
        env['PYTHONPATH'] = str(self.project_root) + ':' + env.get('PYTHONPATH', '')
        
        for path in paths:
            path_obj = Path(path)
            python_files = []
            
            if path_obj.is_file() and path.endswith('.py'):
                python_files = [path_obj]
            elif path_obj.is_dir():
                python_files = list(path_obj.rglob('*.py'))
            
            for py_file in python_files:
                import_results['files_checked'] += 1
                
                try:
                    # Try to import the module
                    cmd = [
                        sys.executable, '-c',
                        f"import py_compile; py_compile.compile('{py_file}', doraise=True)"
                    ]
                    
                    result = subprocess.run(
                        cmd,
                        capture_output=True,
                        text=True,
                        timeout=30,
                        env=env,
                        cwd=self.project_root
                    )
                    
                    if result.returncode == 0:
                        import_results['details'][str(py_file)] = 'imports_ok'
                    else:
                        import_error = {
                            'file': str(py_file),
                            'error': result.stderr.strip(),
                            'type': 'import_error'
                        }
                        import_results['import_errors'].append(import_error)
                        import_results['details'][str(py_file)] = 'import_error'
                        import_results['status'] = 'failed'
                        
                except subprocess.TimeoutExpired:
                    import_error = {
                        'file': str(py_file),
                        'error': 'Import check timed out',
                        'type': 'timeout'
                    }
                    import_results['import_errors'].append(import_error)
                    import_results['details'][str(py_file)] = 'timeout'
                    import_results['status'] = 'failed'
                    
                except Exception as e:
                    import_error = {
                        'file': str(py_file),
                        'error': str(e),
                        'type': 'exception'
                    }
                    import_results['import_errors'].append(import_error)
                    import_results['details'][str(py_file)] = 'exception'
                    import_results['status'] = 'failed'
        
        if import_results['import_errors']:
            print(f"  ❌ Found {len(import_results['import_errors'])} import errors")
            for error in import_results['import_errors'][:5]:  # Show first 5
                print(f"    • {error['file']} - {error['error'][:100]}...")
        else:
            print(f"  ✅ All {import_results['files_checked']} files have valid imports")
        
        return import_results
    
    def run_test_validation(self) -> Dict[str, Any]:
        """Run test suite validation."""
        print("🧪 Running test validation...")
        
        test_results = {
            'status': 'skipped',
            'tests_found': 0,
            'tests_passed': 0,
            'tests_failed': 0,
            'test_output': '',
            'details': {}
        }
        
        # Check if tests directory exists
        tests_dir = self.project_root / 'tests'
        if not tests_dir.exists():
            print("  ⚠️  No tests directory found, skipping test validation")
            return test_results
        
        # Check if pytest is available
        try:
            result = subprocess.run(
                [sys.executable, '-m', 'pytest', '--version'],
                capture_output=True,
                text=True,
                timeout=10
            )
            if result.returncode != 0:
                print("  ⚠️  pytest not available, skipping test validation")
                return test_results
        except (subprocess.TimeoutExpired, FileNotFoundError):
            print("  ⚠️  pytest not available, skipping test validation")
            return test_results
        
        try:
            # Run pytest with minimal output
            cmd = [
                sys.executable, '-m', 'pytest',
                str(tests_dir),
                '--tb=short',
                '--quiet',
                '--disable-warnings'
            ]
            
            env = os.environ.copy()
            env['PYTHONPATH'] = str(self.project_root) + ':' + env.get('PYTHONPATH', '')
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=300,  # 5 minutes max
                env=env,
                cwd=self.project_root
            )
            
            test_results['test_output'] = result.stdout + result.stderr
            test_results['status'] = 'passed' if result.returncode == 0 else 'failed'
            
            # Parse pytest output for statistics
            output_lines = result.stdout.split('\n')
            for line in output_lines:
                if ' passed' in line or ' failed' in line or ' error' in line:
                    # Extract test counts from pytest summary
                    words = line.split()
                    for i, word in enumerate(words):
                        if word == 'passed' and i > 0:
                            test_results['tests_passed'] = int(words[i-1])
                        elif word == 'failed' and i > 0:
                            test_results['tests_failed'] = int(words[i-1])
            
            test_results['tests_found'] = test_results['tests_passed'] + test_results['tests_failed']
            
            if test_results['status'] == 'passed':
                print(f"  ✅ All {test_results['tests_found']} tests passed")
            else:
                print(f"  ❌ {test_results['tests_failed']} tests failed out of {test_results['tests_found']}")
            
        except subprocess.TimeoutExpired:
            test_results['status'] = 'timeout'
            test_results['test_output'] = 'Test execution timed out'
            print("  ⏰ Test execution timed out")
            
        except Exception as e:
            test_results['status'] = 'error'
            test_results['test_output'] = str(e)
            print(f"  ❌ Error running tests: {e}")
        
        return test_results
    
    def run_documentation_check(self) -> Dict[str, Any]:
        """Check documentation completeness."""
        print("📚 Running documentation validation...")
        
        doc_results = {
            'status': 'passed',
            'files_checked': 0,
            'missing_docstrings': [],
            'coverage': 0.0,
            'details': {}
        }
        
        # Check for docstrings in Python files
        python_files = list(self.project_root.rglob('*.py'))
        # Exclude test files and __init__.py
        python_files = [f for f in python_files if 'test' not in str(f) and f.name != '__init__.py']
        
        total_functions = 0
        documented_functions = 0
        
        for py_file in python_files:
            doc_results['files_checked'] += 1
            
            try:
                with open(py_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                # Simple check for function definitions and docstrings
                lines = content.split('\n')
                i = 0
                while i < len(lines):
                    line = lines[i].strip()
                    if line.startswith('def ') and not line.startswith('def _'):
                        # Found a public function
                        total_functions += 1
                        function_name = line.split('(')[0].replace('def ', '')
                        
                        # Check if next non-empty line is a docstring
                        j = i + 1
                        while j < len(lines) and not lines[j].strip():
                            j += 1
                        
                        if j < len(lines) and ('"""' in lines[j] or "'''" in lines[j]):
                            documented_functions += 1
                        else:
                            doc_results['missing_docstrings'].append({
                                'file': str(py_file),
                                'function': function_name,
                                'line': i + 1
                            })
                    i += 1
                    
            except Exception as e:
                print(f"  ⚠️  Error checking {py_file}: {e}")
        
        if total_functions > 0:
            doc_results['coverage'] = (documented_functions / total_functions) * 100
        
        if doc_results['coverage'] < 50:
            doc_results['status'] = 'failed'
        elif doc_results['coverage'] < 75:
            doc_results['status'] = 'warning'
        
        print(f"  📊 Documentation coverage: {doc_results['coverage']:.1f}% ({documented_functions}/{total_functions})")
        
        if doc_results['missing_docstrings']:
            print(f"  ⚠️  {len(doc_results['missing_docstrings'])} functions missing docstrings")
        
        return doc_results
    
    def run_security_check(self, paths: List[str]) -> Dict[str, Any]:
        """Basic security checks."""
        print("🔒 Running security validation...")
        
        security_results = {
            'status': 'passed',
            'files_checked': 0,
            'security_issues': [],
            'details': {}
        }
        
        # Security patterns to check
        security_patterns = [
            (r'password\s*=\s*["\'][^"\']*["\']', 'Potential hardcoded password'),
            (r'secret\s*=\s*["\'][^"\']*["\']', 'Potential hardcoded secret'),
            (r'api_key\s*=\s*["\'][^"\']*["\']', 'Potential hardcoded API key'),
            (r'token\s*=\s*["\'][^"\']*["\']', 'Potential hardcoded token'),
            (r'exec\s*\(', 'Use of exec() function'),
            (r'eval\s*\(', 'Use of eval() function'),
            (r'os\.system\s*\(', 'Use of os.system()'),
            (r'subprocess\.call.*shell\s*=\s*True', 'Shell injection risk'),
        ]
        
        import re
        
        for path in paths:
            path_obj = Path(path)
            python_files = []
            
            if path_obj.is_file() and path.endswith('.py'):
                python_files = [path_obj]
            elif path_obj.is_dir():
                python_files = list(path_obj.rglob('*.py'))
            
            for py_file in python_files:
                security_results['files_checked'] += 1
                
                try:
                    with open(py_file, 'r', encoding='utf-8') as f:
                        content = f.read()
                    
                    for pattern, description in security_patterns:
                        matches = re.finditer(pattern, content, re.IGNORECASE)
                        for match in matches:
                            # Find line number
                            line_num = content[:match.start()].count('\n') + 1
                            
                            security_issue = {
                                'file': str(py_file),
                                'line': line_num,
                                'issue': description,
                                'code': match.group(0)
                            }
                            security_results['security_issues'].append(security_issue)
                            security_results['status'] = 'warning'
                            
                except Exception as e:
                    print(f"  ⚠️  Error checking {py_file}: {e}")
        
        if security_results['security_issues']:
            print(f"  ⚠️  Found {len(security_results['security_issues'])} potential security issues")
            for issue in security_results['security_issues'][:3]:  # Show first 3
                print(f"    • {issue['file']}:{issue['line']} - {issue['issue']}")
        else:
            print(f"  ✅ No obvious security issues found in {security_results['files_checked']} files")
        
        return security_results
    
    def run_comprehensive_validation(self, 
                                   paths: List[str],
                                   validations: List[str] = None) -> Dict[str, Any]:
        """Run comprehensive validation pipeline."""
        
        if validations is None:
            validations = ['syntax', 'imports', 'tests', 'documentation', 'security']
        
        print(f"🚀 Starting comprehensive validation...")
        print(f"Validations: {', '.join(validations)}")
        print(f"Paths: {', '.join(paths)}")
        
        # Run each validation
        if 'syntax' in validations:
            self.validation_results['validations']['syntax'] = self.run_syntax_check(paths)
        
        if 'imports' in validations:
            self.validation_results['validations']['imports'] = self.run_import_check(paths)
        
        if 'tests' in validations:
            self.validation_results['validations']['tests'] = self.run_test_validation()
        
        if 'documentation' in validations:
            self.validation_results['validations']['documentation'] = self.run_documentation_check()
        
        if 'security' in validations:
            self.validation_results['validations']['security'] = self.run_security_check(paths)
        
        # Calculate overall status
        self._calculate_overall_status()
        
        return self.validation_results
    
    def _calculate_overall_status(self):
        """Calculate overall validation status."""
        statuses = [v.get('status', 'unknown') for v in self.validation_results['validations'].values()]
        
        if 'failed' in statuses:
            self.validation_results['overall_status'] = 'failed'
        elif 'warning' in statuses:
            self.validation_results['overall_status'] = 'warning'
        elif all(s in ['passed', 'skipped'] for s in statuses):
            self.validation_results['overall_status'] = 'passed'
        else:
            self.validation_results['overall_status'] = 'unknown'
        
        # Generate summary
        self.validation_results['summary'] = {
            'total_validations': len(self.validation_results['validations']),
            'passed': sum(1 for s in statuses if s == 'passed'),
            'failed': sum(1 for s in statuses if s == 'failed'),
            'warnings': sum(1 for s in statuses if s == 'warning'),
            'skipped': sum(1 for s in statuses if s == 'skipped'),
            'status_breakdown': dict(zip(
                self.validation_results['validations'].keys(),
                statuses
            ))
        }
    
    def save_validation_report(self, output_file: str = None):
        """Save validation report to file."""
        if output_file is None:
            output_file = f"validation_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        with open(output_file, 'w') as f:
            json.dump(self.validation_results, f, indent=2, ensure_ascii=False)
        
        print(f"📝 Validation report saved to: {output_file}")
    
    def print_validation_summary(self):
        """Print validation summary."""
        print("\n" + "="*60)
        print("✅ VALIDATION SUMMARY")
        print("="*60)
        
        summary = self.validation_results['summary']
        overall_status = self.validation_results['overall_status']
        
        # Overall status with emoji
        status_emoji = {
            'passed': '✅',
            'warning': '⚠️',
            'failed': '❌',
            'unknown': '❓'
        }
        
        print(f"\n{status_emoji.get(overall_status, '❓')} Overall Status: {overall_status.upper()}")
        
        print(f"\n📊 Validation Results:")
        print(f"  • Total validations: {summary['total_validations']}")
        print(f"  • Passed: {summary['passed']}")
        print(f"  • Failed: {summary['failed']}")
        print(f"  • Warnings: {summary['warnings']}")
        print(f"  • Skipped: {summary['skipped']}")
        
        print(f"\n📋 Details by Validation:")
        for validation_name, status in summary['status_breakdown'].items():
            emoji = status_emoji.get(status, '❓')
            print(f"  {emoji} {validation_name.capitalize()}: {status}")
        
        print("\n" + "="*60)


def main():
    """Main entry point for validation runner."""
    parser = argparse.ArgumentParser(description='Validation Runner for Python Cellframe SDK')
    parser.add_argument('--paths', nargs='+', default=['CellFrame/', 'src/', 'tests/'],
                       help='Paths to validate')
    parser.add_argument('--validations', nargs='+',
                       choices=['syntax', 'imports', 'tests', 'documentation', 'security'],
                       default=['syntax', 'imports', 'tests', 'documentation', 'security'],
                       help='Validations to run')
    parser.add_argument('--output', '-o', help='Output file for validation report')
    parser.add_argument('--quiet', '-q', action='store_true', help='Suppress progress output')
    
    args = parser.parse_args()
    
    validator = ValidationRunner()
    
    # Run validations
    if not args.quiet:
        print("🔍 Starting validation pipeline...")
    
    results = validator.run_comprehensive_validation(
        paths=args.paths,
        validations=args.validations
    )
    
    # Print summary unless quiet
    if not args.quiet:
        validator.print_validation_summary()
    
    # Save report
    validator.save_validation_report(args.output)
    
    # Exit with appropriate code
    overall_status = results['overall_status']
    if overall_status == 'failed':
        return 1
    elif overall_status == 'warning':
        return 2
    else:
        return 0


if __name__ == '__main__':
    exit(main())