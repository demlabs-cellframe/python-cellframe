#!/usr/bin/env python3
"""
🔄 Iterative Error Fixing Pipeline for Python Cellframe SDK
Iterative cycle of detection -> classification -> fixing -> validation.
"""

import os
import sys
import json
import subprocess
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
from datetime import datetime


class IterativeFixer:
    """Iterative error fixing pipeline."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.iteration_count = 0
        self.max_iterations = 5
        self.iteration_history = []
    
    def run_iteration_cycle(self, 
                          paths: List[str],
                          max_iterations: int = 5) -> Dict[str, Any]:
        """Run complete iterative fixing cycle."""
        
        self.max_iterations = max_iterations
        cycle_results = {
            'start_time': datetime.now().isoformat(),
            'iterations': [],
            'final_status': 'unknown',
            'summary': {}
        }
        
        print(f"🔄 Starting iterative fixing cycle (max {max_iterations} iterations)")
        print(f"Paths: {', '.join(paths)}")
        
        for iteration in range(1, max_iterations + 1):
            print(f"\n{'='*60}")
            print(f"🔄 ITERATION {iteration}")
            print(f"{'='*60}")
            
            iteration_result = self._run_single_iteration(paths, iteration)
            cycle_results['iterations'].append(iteration_result)
            
            # Check if we should continue
            if iteration_result['should_continue'] == False:
                print(f"\n✅ Stopping iterations: {iteration_result['stop_reason']}")
                break
            
            if iteration_result['issues_fixed'] == 0 and iteration > 1:
                print(f"\n⏸️  No progress made in iteration {iteration}, stopping")
                break
        
        # Generate final summary
        cycle_results['end_time'] = datetime.now().isoformat()
        cycle_results['total_iterations'] = len(cycle_results['iterations'])
        cycle_results['summary'] = self._generate_cycle_summary(cycle_results['iterations'])
        
        # Final validation
        print(f"\n🏁 Running final validation...")
        final_validation = self._run_final_validation(paths)
        cycle_results['final_validation'] = final_validation
        cycle_results['final_status'] = final_validation.get('overall_status', 'unknown')
        
        return cycle_results
    
    def _run_single_iteration(self, paths: List[str], iteration_num: int) -> Dict[str, Any]:
        """Run a single iteration of the fixing cycle."""
        
        iteration_result = {
            'iteration': iteration_num,
            'start_time': datetime.now().isoformat(),
            'steps': {},
            'issues_detected': 0,
            'issues_fixed': 0,
            'should_continue': True,
            'stop_reason': '',
            'status': 'unknown'
        }
        
        try:
            # Step 1: Detection (static analysis + validation)
            print(f"🔍 Step 1: Error Detection")
            detection_result = self._run_detection(paths)
            iteration_result['steps']['detection'] = detection_result
            iteration_result['issues_detected'] = detection_result.get('total_issues', 0)
            
            # Step 2: Classification  
            print(f"🏷️  Step 2: Error Classification")
            if detection_result.get('total_issues', 0) > 0:
                classification_result = self._run_classification(detection_result)
                iteration_result['steps']['classification'] = classification_result
            else:
                print(f"  ✅ No issues to classify")
                iteration_result['should_continue'] = False
                iteration_result['stop_reason'] = 'No issues detected'
                iteration_result['status'] = 'completed'
                return iteration_result
            
            # Step 3: Fixing
            print(f"🔧 Step 3: Automatic Fixing")
            fixing_result = self._run_fixing(paths, classification_result)
            iteration_result['steps']['fixing'] = fixing_result
            iteration_result['issues_fixed'] = fixing_result.get('fixes_applied_count', 0)
            
            # Step 4: Validation
            print(f"✅ Step 4: Validation")
            validation_result = self._run_validation(paths)
            iteration_result['steps']['validation'] = validation_result
            
            # Determine if we should continue
            if validation_result.get('overall_status') == 'passed':
                iteration_result['should_continue'] = False
                iteration_result['stop_reason'] = 'All validations passed'
                iteration_result['status'] = 'completed'
            elif iteration_result['issues_fixed'] == 0:
                iteration_result['should_continue'] = False  
                iteration_result['stop_reason'] = 'No fixable issues remaining'
                iteration_result['status'] = 'stalled'
            else:
                iteration_result['status'] = 'progress'
            
        except Exception as e:
            iteration_result['status'] = 'error'
            iteration_result['error'] = str(e)
            iteration_result['should_continue'] = False
            iteration_result['stop_reason'] = f'Error occurred: {e}'
            print(f"❌ Error in iteration {iteration_num}: {e}")
        
        iteration_result['end_time'] = datetime.now().isoformat()
        
        # Print iteration summary
        print(f"\n📊 Iteration {iteration_num} Summary:")
        print(f"  • Issues detected: {iteration_result['issues_detected']}")
        print(f"  • Issues fixed: {iteration_result['issues_fixed']}")
        print(f"  • Status: {iteration_result['status']}")
        print(f"  • Continue: {iteration_result['should_continue']}")
        
        return iteration_result
    
    def _run_detection(self, paths: List[str]) -> Dict[str, Any]:
        """Run error detection (static analysis + basic validation)."""
        
        # Run validation for immediate issues
        try:
            cmd = [
                sys.executable, 'scripts/validation_runner.py',
                '--paths'] + paths + [
                '--validations', 'syntax', 'imports', 'security',
                '--quiet'
            ]
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=180,
                cwd=self.project_root
            )
            
            # Try to parse validation output file
            validation_files = list(self.project_root.glob('validation_report_*.json'))
            if validation_files:
                latest_file = max(validation_files, key=lambda f: f.stat().st_mtime)
                with open(latest_file, 'r') as f:
                    validation_data = json.load(f)
                
                # Count total issues from validation
                total_issues = 0
                for validation_name, validation_result in validation_data.get('validations', {}).items():
                    if isinstance(validation_result, dict):
                        # Count different types of issues
                        total_issues += len(validation_result.get('syntax_errors', []))
                        total_issues += len(validation_result.get('import_errors', []))
                        total_issues += len(validation_result.get('security_issues', []))
                        total_issues += len(validation_result.get('missing_docstrings', []))
                
                return {
                    'method': 'validation',
                    'total_issues': total_issues,
                    'validation_data': validation_data,
                    'status': 'completed'
                }
            
        except Exception as e:
            print(f"  ⚠️  Validation detection failed: {e}")
        
        # Fallback to basic file existence check
        python_files = []
        for path in paths:
            path_obj = Path(path)
            if path_obj.is_file() and path.endswith('.py'):
                python_files.append(path_obj)
            elif path_obj.is_dir():
                python_files.extend(list(path_obj.rglob('*.py')))
        
        return {
            'method': 'basic',
            'total_issues': 0,  # No real detection in basic mode
            'files_found': len(python_files),
            'status': 'completed'
        }
    
    def _run_classification(self, detection_result: Dict[str, Any]) -> Dict[str, Any]:
        """Run error classification."""
        
        if detection_result.get('method') == 'validation':
            validation_data = detection_result.get('validation_data', {})
            
            # Simple classification based on validation results
            classifications = {
                'critical': [],
                'high': [],
                'medium': [],
                'low': [],
                'auto_fixable': []
            }
            
            # Classify syntax errors as critical
            for validation_name, validation_result in validation_data.get('validations', {}).items():
                if validation_name == 'syntax':
                    for error in validation_result.get('syntax_errors', []):
                        classifications['critical'].append({
                            'type': 'syntax_error',
                            'file': error.get('file'),
                            'line': error.get('line'),
                            'message': error.get('message')
                        })
                
                elif validation_name == 'imports':
                    for error in validation_result.get('import_errors', []):
                        classifications['high'].append({
                            'type': 'import_error',
                            'file': error.get('file'),
                            'error': error.get('error')
                        })
                
                elif validation_name == 'security':
                    for issue in validation_result.get('security_issues', []):
                        classifications['medium'].append({
                            'type': 'security_issue',
                            'file': issue.get('file'),
                            'line': issue.get('line'),
                            'issue': issue.get('issue')
                        })
                
                elif validation_name == 'documentation':
                    for missing in validation_result.get('missing_docstrings', []):
                        classifications['low'].append({
                            'type': 'missing_docstring',
                            'file': missing.get('file'),
                            'function': missing.get('function'),
                            'line': missing.get('line')
                        })
            
            total_classified = sum(len(issues) for issues in classifications.values())
            
            return {
                'method': 'validation_based',
                'classifications': classifications,
                'total_classified': total_classified,
                'status': 'completed'
            }
        
        return {
            'method': 'none',
            'total_classified': 0,
            'status': 'skipped'
        }
    
    def _run_fixing(self, paths: List[str], classification_result: Dict[str, Any]) -> Dict[str, Any]:
        """Run automatic fixing."""
        
        fixing_result = {
            'method': 'auto_fixer',
            'fixes_applied_count': 0,
            'files_modified': 0,
            'status': 'completed'
        }
        
        try:
            # Run auto fixer
            cmd = [
                sys.executable, 'scripts/auto_fixer.py',
                '--paths'] + paths + [
                '--fixes', 'unused_imports', 'whitespace', 'import_order'
            ]
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=300,
                cwd=self.project_root
            )
            
            # Try to parse auto-fixer output
            if 'fixes applied:' in result.stdout.lower():
                # Extract number of fixes from output
                lines = result.stdout.split('\n')
                for line in lines:
                    if 'total fixes applied:' in line.lower():
                        try:
                            fixing_result['fixes_applied_count'] = int(line.split(':')[-1].strip())
                        except ValueError:
                            pass
                    elif 'files modified:' in line.lower():
                        try:
                            fixing_result['files_modified'] = int(line.split(':')[-1].strip())
                        except ValueError:
                            pass
            
            print(f"  🔧 Applied {fixing_result['fixes_applied_count']} fixes to {fixing_result['files_modified']} files")
            
        except Exception as e:
            print(f"  ⚠️  Auto-fixing failed: {e}")
            fixing_result['status'] = 'failed'
            fixing_result['error'] = str(e)
        
        return fixing_result
    
    def _run_validation(self, paths: List[str]) -> Dict[str, Any]:
        """Run validation after fixing."""
        
        try:
            cmd = [
                sys.executable, 'scripts/validation_runner.py',
                '--paths'] + paths + [
                '--validations', 'syntax', 'imports',
                '--quiet'
            ]
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=120,
                cwd=self.project_root
            )
            
            # Check validation result
            if result.returncode == 0:
                overall_status = 'passed'
            elif result.returncode == 2:
                overall_status = 'warning'
            else:
                overall_status = 'failed'
            
            return {
                'overall_status': overall_status,
                'return_code': result.returncode,
                'status': 'completed'
            }
            
        except Exception as e:
            print(f"  ⚠️  Validation failed: {e}")
            return {
                'overall_status': 'error',
                'error': str(e),
                'status': 'failed'
            }
    
    def _run_final_validation(self, paths: List[str]) -> Dict[str, Any]:
        """Run comprehensive final validation."""
        
        try:
            cmd = [
                sys.executable, 'scripts/validation_runner.py',
                '--paths'] + paths + [
                '--validations', 'syntax', 'imports', 'documentation', 'security'
            ]
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=300,
                cwd=self.project_root
            )
            
            # Parse final validation report
            validation_files = list(self.project_root.glob('validation_report_*.json'))
            if validation_files:
                latest_file = max(validation_files, key=lambda f: f.stat().st_mtime)
                with open(latest_file, 'r') as f:
                    validation_data = json.load(f)
                
                return validation_data
            
        except Exception as e:
            print(f"  ⚠️  Final validation failed: {e}")
        
        return {
            'overall_status': 'error',
            'error': 'Final validation failed'
        }
    
    def _generate_cycle_summary(self, iterations: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Generate summary of the entire cycle."""
        
        total_issues_detected = sum(
            iteration.get('issues_detected', 0) for iteration in iterations
        )
        total_issues_fixed = sum(
            iteration.get('issues_fixed', 0) for iteration in iterations
        )
        
        # Find the final status
        final_iteration = iterations[-1] if iterations else {}
        final_status = final_iteration.get('status', 'unknown')
        
        return {
            'total_iterations': len(iterations),
            'total_issues_detected': total_issues_detected,
            'total_issues_fixed': total_issues_fixed,
            'final_iteration_status': final_status,
            'improvement_rate': (total_issues_fixed / total_issues_detected * 100) if total_issues_detected > 0 else 0,
            'iterations_summary': [
                {
                    'iteration': i.get('iteration'),
                    'detected': i.get('issues_detected', 0),
                    'fixed': i.get('issues_fixed', 0),
                    'status': i.get('status')
                }
                for i in iterations
            ]
        }
    
    def save_cycle_report(self, cycle_results: Dict[str, Any], output_file: str = None):
        """Save cycle report to file."""
        if output_file is None:
            output_file = f"iterative_fixing_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        with open(output_file, 'w') as f:
            json.dump(cycle_results, f, indent=2, ensure_ascii=False)
        
        print(f"📝 Iterative fixing report saved to: {output_file}")
    
    def print_cycle_summary(self, cycle_results: Dict[str, Any]):
        """Print comprehensive cycle summary."""
        print("\n" + "="*60)
        print("🔄 ITERATIVE FIXING CYCLE SUMMARY")
        print("="*60)
        
        summary = cycle_results.get('summary', {})
        final_validation = cycle_results.get('final_validation', {})
        
        print(f"\n📊 Cycle Statistics:")
        print(f"  • Total iterations: {summary.get('total_iterations', 0)}")
        print(f"  • Issues detected: {summary.get('total_issues_detected', 0)}")
        print(f"  • Issues fixed: {summary.get('total_issues_fixed', 0)}")
        print(f"  • Improvement rate: {summary.get('improvement_rate', 0):.1f}%")
        
        print(f"\n🏁 Final Status: {cycle_results.get('final_status', 'unknown').upper()}")
        
        if summary.get('iterations_summary'):
            print(f"\n📈 Iteration Progress:")
            for iter_summary in summary['iterations_summary']:
                print(f"  • Iteration {iter_summary['iteration']}: "
                      f"{iter_summary['detected']} detected, "
                      f"{iter_summary['fixed']} fixed, "
                      f"status: {iter_summary['status']}")
        
        # Final validation summary
        if final_validation.get('summary'):
            final_summary = final_validation['summary']
            print(f"\n✅ Final Validation:")
            print(f"  • Passed: {final_summary.get('passed', 0)}")
            print(f"  • Failed: {final_summary.get('failed', 0)}")
            print(f"  • Warnings: {final_summary.get('warnings', 0)}")
            print(f"  • Skipped: {final_summary.get('skipped', 0)}")
        
        print("\n" + "="*60)


def main():
    """Main entry point for iterative fixer."""
    parser = argparse.ArgumentParser(description='Iterative Error Fixing for Python Cellframe SDK')
    parser.add_argument('--paths', nargs='+', default=['CellFrame/', 'tests/'],
                       help='Paths to fix iteratively')
    parser.add_argument('--max-iterations', type=int, default=5,
                       help='Maximum number of iterations')
    parser.add_argument('--output', '-o', help='Output file for cycle report')
    parser.add_argument('--quiet', '-q', action='store_true', help='Suppress progress output')
    
    args = parser.parse_args()
    
    fixer = IterativeFixer()
    
    if not args.quiet:
        print("🔄 Starting iterative fixing pipeline...")
    
    # Run iterative cycle
    cycle_results = fixer.run_iteration_cycle(
        paths=args.paths,
        max_iterations=args.max_iterations
    )
    
    # Print summary unless quiet
    if not args.quiet:
        fixer.print_cycle_summary(cycle_results)
    
    # Save report
    fixer.save_cycle_report(cycle_results, args.output)
    
    # Exit with appropriate code based on final status
    final_status = cycle_results.get('final_status', 'unknown')
    if final_status == 'passed':
        return 0
    elif final_status in ['warning', 'stalled']:
        return 1
    else:
        return 2


if __name__ == '__main__':
    exit(main())