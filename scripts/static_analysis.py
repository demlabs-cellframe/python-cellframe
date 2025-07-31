#!/usr/bin/env python3
"""
🔍 Static Analysis Script for Python Cellframe SDK
Automated detection and classification of code issues.
"""

import os
import sys
import subprocess
import json
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
from datetime import datetime
import tempfile


class StaticAnalyzer:
    """Comprehensive static analysis for Python Cellframe SDK."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.issues = []
        self.tools_available = {}
        
        # Check which tools are available
        self._check_tool_availability()
    
    def _check_tool_availability(self):
        """Check which static analysis tools are available."""
        tools = {
            'pylint': 'pylint --version',
            'flake8': 'flake8 --version',
            'mypy': 'mypy --version',
            'bandit': 'bandit --version',
            'safety': 'safety --version',
            'black': 'black --version',
            'isort': 'isort --version',
            'vulture': 'vulture --version',
            'radon': 'radon --version',
            'mccabe': 'python -c "import mccabe; print(mccabe.__version__)"'
        }
        
        for tool, check_cmd in tools.items():
            try:
                result = subprocess.run(
                    check_cmd.split(), 
                    capture_output=True, 
                    text=True, 
                    timeout=10
                )
                self.tools_available[tool] = result.returncode == 0
            except (subprocess.TimeoutExpired, FileNotFoundError):
                self.tools_available[tool] = False
        
        print("🔧 Available static analysis tools:")
        for tool, available in self.tools_available.items():
            status = "✅" if available else "❌"
            print(f"  {status} {tool}")
    
    def run_pylint(self, paths: List[str]) -> List[Dict[str, Any]]:
        """Run pylint analysis."""
        if not self.tools_available.get('pylint', False):
            return []
        
        issues = []
        print("🔍 Running pylint...")
        
        for path in paths:
            if not Path(path).exists():
                continue
                
            try:
                cmd = [
                    'pylint', 
                    '--output-format=json',
                    '--disable=C0114,C0115,C0116',  # Disable docstring warnings for now
                    '--disable=R0903,R0913',  # Disable some design warnings
                    str(path)
                ]
                
                result = subprocess.run(
                    cmd, 
                    capture_output=True, 
                    text=True, 
                    timeout=60
                )
                
                if result.stdout:
                    pylint_issues = json.loads(result.stdout)
                    for issue in pylint_issues:
                        issues.append({
                            'tool': 'pylint',
                            'type': issue.get('type', 'unknown'),
                            'message': issue.get('message', ''),
                            'symbol': issue.get('symbol', ''),
                            'file': issue.get('path', ''),
                            'line': issue.get('line', 0),
                            'column': issue.get('column', 0),
                            'severity': self._map_pylint_severity(issue.get('type', '')),
                            'category': 'code_quality'
                        })
                        
            except (subprocess.TimeoutExpired, json.JSONDecodeError, Exception) as e:
                print(f"⚠️  Pylint error for {path}: {e}")
        
        return issues
    
    def run_flake8(self, paths: List[str]) -> List[Dict[str, Any]]:
        """Run flake8 analysis."""
        if not self.tools_available.get('flake8', False):
            return []
        
        issues = []
        print("🔍 Running flake8...")
        
        for path in paths:
            if not Path(path).exists():
                continue
                
            try:
                cmd = [
                    'flake8', 
                    '--format=json',
                    '--max-line-length=100',
                    '--ignore=E203,W503,E501',  # Ignore some formatting issues
                    str(path)
                ]
                
                result = subprocess.run(
                    cmd, 
                    capture_output=True, 
                    text=True, 
                    timeout=30
                )
                
                if result.stdout:
                    try:
                        flake8_data = json.loads(result.stdout)
                        for file_path, file_issues in flake8_data.items():
                            for issue in file_issues:
                                issues.append({
                                    'tool': 'flake8',
                                    'type': 'style',
                                    'message': issue.get('text', ''),
                                    'code': issue.get('code', ''),
                                    'file': file_path,
                                    'line': issue.get('line_number', 0),
                                    'column': issue.get('column_number', 0),
                                    'severity': 'low',
                                    'category': 'style'
                                })
                    except json.JSONDecodeError:
                        # Fallback to text parsing
                        for line in result.stdout.split('\n'):
                            if ':' in line and len(line.strip()) > 0:
                                parts = line.split(':')
                                if len(parts) >= 4:
                                    issues.append({
                                        'tool': 'flake8',
                                        'type': 'style',
                                        'message': ':'.join(parts[3:]).strip(),
                                        'file': parts[0],
                                        'line': int(parts[1]) if parts[1].isdigit() else 0,
                                        'column': int(parts[2]) if parts[2].isdigit() else 0,
                                        'severity': 'low',
                                        'category': 'style'
                                    })
                        
            except (subprocess.TimeoutExpired, Exception) as e:
                print(f"⚠️  Flake8 error for {path}: {e}")
        
        return issues
    
    def run_mypy(self, paths: List[str]) -> List[Dict[str, Any]]:
        """Run mypy type checking."""
        if not self.tools_available.get('mypy', False):
            return []
        
        issues = []
        print("🔍 Running mypy...")
        
        for path in paths:
            if not Path(path).exists():
                continue
                
            try:
                cmd = [
                    'mypy', 
                    '--no-error-summary',
                    '--ignore-missing-imports',
                    '--follow-imports=silent',
                    str(path)
                ]
                
                result = subprocess.run(
                    cmd, 
                    capture_output=True, 
                    text=True, 
                    timeout=60
                )
                
                for line in result.stdout.split('\n'):
                    if ':' in line and 'error:' in line:
                        parts = line.split(':')
                        if len(parts) >= 3:
                            try:
                                file_path = parts[0]
                                line_num = int(parts[1]) if parts[1].isdigit() else 0
                                message = ':'.join(parts[2:]).replace('error:', '').strip()
                                
                                issues.append({
                                    'tool': 'mypy',
                                    'type': 'type_error',
                                    'message': message,
                                    'file': file_path,
                                    'line': line_num,
                                    'column': 0,
                                    'severity': 'medium',
                                    'category': 'typing'
                                })
                            except ValueError:
                                continue
                        
            except (subprocess.TimeoutExpired, Exception) as e:
                print(f"⚠️  MyPy error for {path}: {e}")
        
        return issues
    
    def run_bandit(self, paths: List[str]) -> List[Dict[str, Any]]:
        """Run bandit security analysis."""
        if not self.tools_available.get('bandit', False):
            return []
        
        issues = []
        print("🔍 Running bandit...")
        
        for path in paths:
            if not Path(path).exists():
                continue
                
            try:
                cmd = [
                    'bandit', 
                    '-f', 'json',
                    '-r',
                    str(path)
                ]
                
                result = subprocess.run(
                    cmd, 
                    capture_output=True, 
                    text=True, 
                    timeout=60
                )
                
                if result.stdout:
                    bandit_data = json.loads(result.stdout)
                    for issue in bandit_data.get('results', []):
                        issues.append({
                            'tool': 'bandit',
                            'type': 'security',
                            'message': issue.get('issue_text', ''),
                            'test_id': issue.get('test_id', ''),
                            'file': issue.get('filename', ''),
                            'line': issue.get('line_number', 0),
                            'column': 0,
                            'severity': issue.get('issue_severity', 'medium').lower(),
                            'confidence': issue.get('issue_confidence', 'medium').lower(),
                            'category': 'security'
                        })
                        
            except (subprocess.TimeoutExpired, json.JSONDecodeError, Exception) as e:
                print(f"⚠️  Bandit error for {path}: {e}")
        
        return issues
    
    def run_vulture(self, paths: List[str]) -> List[Dict[str, Any]]:
        """Run vulture dead code analysis."""
        if not self.tools_available.get('vulture', False):
            return []
        
        issues = []
        print("🔍 Running vulture...")
        
        for path in paths:
            if not Path(path).exists():
                continue
                
            try:
                cmd = ['vulture', str(path)]
                
                result = subprocess.run(
                    cmd, 
                    capture_output=True, 
                    text=True, 
                    timeout=30
                )
                
                for line in result.stdout.split('\n'):
                    if ':' in line and 'unused' in line.lower():
                        parts = line.split(':')
                        if len(parts) >= 3:
                            try:
                                file_path = parts[0]
                                line_num = int(parts[1]) if parts[1].isdigit() else 0
                                message = ':'.join(parts[2:]).strip()
                                
                                issues.append({
                                    'tool': 'vulture',
                                    'type': 'dead_code',
                                    'message': message,
                                    'file': file_path,
                                    'line': line_num,
                                    'column': 0,
                                    'severity': 'low',
                                    'category': 'maintainability'
                                })
                            except ValueError:
                                continue
                        
            except (subprocess.TimeoutExpired, Exception) as e:
                print(f"⚠️  Vulture error for {path}: {e}")
        
        return issues
    
    def run_complexity_analysis(self, paths: List[str]) -> List[Dict[str, Any]]:
        """Run complexity analysis with radon."""
        if not self.tools_available.get('radon', False):
            return []
        
        issues = []
        print("🔍 Running complexity analysis...")
        
        for path in paths:
            if not Path(path).exists():
                continue
                
            try:
                # Cyclomatic complexity
                cmd = ['radon', 'cc', '-j', str(path)]
                
                result = subprocess.run(
                    cmd, 
                    capture_output=True, 
                    text=True, 
                    timeout=30
                )
                
                if result.stdout:
                    complexity_data = json.loads(result.stdout)
                    for file_path, functions in complexity_data.items():
                        for func in functions:
                            complexity = func.get('complexity', 0)
                            if complexity > 10:  # High complexity threshold
                                issues.append({
                                    'tool': 'radon',
                                    'type': 'complexity',
                                    'message': f"High cyclomatic complexity: {complexity}",
                                    'function': func.get('name', ''),
                                    'file': file_path,
                                    'line': func.get('lineno', 0),
                                    'column': 0,
                                    'severity': 'medium' if complexity > 15 else 'low',
                                    'category': 'maintainability',
                                    'complexity_score': complexity
                                })
                        
            except (subprocess.TimeoutExpired, json.JSONDecodeError, Exception) as e:
                print(f"⚠️  Radon error for {path}: {e}")
        
        return issues
    
    def _map_pylint_severity(self, pylint_type: str) -> str:
        """Map pylint message types to severity levels."""
        mapping = {
            'error': 'high',
            'warning': 'medium', 
            'refactor': 'low',
            'convention': 'low',
            'info': 'low'
        }
        return mapping.get(pylint_type.lower(), 'medium')
    
    def analyze_project(self, 
                       target_paths: Optional[List[str]] = None,
                       tools: Optional[List[str]] = None) -> Dict[str, Any]:
        """Run comprehensive static analysis on the project."""
        
        if target_paths is None:
            target_paths = ['CellFrame/', 'src/', 'tests/']
        
        if tools is None:
            tools = ['pylint', 'flake8', 'mypy', 'bandit', 'vulture', 'radon']
        
        print(f"🔍 Starting static analysis on: {', '.join(target_paths)}")
        
        all_issues = []
        
        # Run each requested tool
        if 'pylint' in tools:
            all_issues.extend(self.run_pylint(target_paths))
        
        if 'flake8' in tools:
            all_issues.extend(self.run_flake8(target_paths))
        
        if 'mypy' in tools:
            all_issues.extend(self.run_mypy(target_paths))
        
        if 'bandit' in tools:
            all_issues.extend(self.run_bandit(target_paths))
        
        if 'vulture' in tools:
            all_issues.extend(self.run_vulture(target_paths))
        
        if 'radon' in tools:
            all_issues.extend(self.run_complexity_analysis(target_paths))
        
        # Classify and summarize issues
        analysis_result = self._classify_issues(all_issues)
        
        return analysis_result
    
    def _classify_issues(self, issues: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Classify issues by severity, category, and file."""
        
        classification = {
            'total_issues': len(issues),
            'by_severity': {'high': 0, 'medium': 0, 'low': 0},
            'by_category': {},
            'by_tool': {},
            'by_file': {},
            'issues': issues,
            'summary': {},
            'timestamp': datetime.now().isoformat()
        }
        
        for issue in issues:
            # Count by severity
            severity = issue.get('severity', 'medium')
            classification['by_severity'][severity] = classification['by_severity'].get(severity, 0) + 1
            
            # Count by category
            category = issue.get('category', 'unknown')
            classification['by_category'][category] = classification['by_category'].get(category, 0) + 1
            
            # Count by tool
            tool = issue.get('tool', 'unknown')
            classification['by_tool'][tool] = classification['by_tool'].get(tool, 0) + 1
            
            # Count by file
            file_path = issue.get('file', 'unknown')
            classification['by_file'][file_path] = classification['by_file'].get(file_path, 0) + 1
        
        # Generate summary
        classification['summary'] = {
            'critical_files': [
                file for file, count in classification['by_file'].items() 
                if count > 10
            ],
            'most_common_categories': sorted(
                classification['by_category'].items(), 
                key=lambda x: x[1], 
                reverse=True
            )[:5],
            'tool_effectiveness': {
                tool: count for tool, count in classification['by_tool'].items()
            }
        }
        
        return classification
    
    def save_report(self, analysis_result: Dict[str, Any], output_file: str = None):
        """Save analysis report to file."""
        if output_file is None:
            output_file = f"static_analysis_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        with open(output_file, 'w') as f:
            json.dump(analysis_result, f, indent=2, ensure_ascii=False)
        
        print(f"📝 Analysis report saved to: {output_file}")
    
    def print_summary(self, analysis_result: Dict[str, Any]):
        """Print analysis summary to console."""
        print("\n" + "="*60)
        print("📊 STATIC ANALYSIS SUMMARY")
        print("="*60)
        
        print(f"\n🔢 Total Issues: {analysis_result['total_issues']}")
        
        print("\n📈 By Severity:")
        for severity, count in analysis_result['by_severity'].items():
            if count > 0:
                emoji = {"high": "🔴", "medium": "🟡", "low": "🟢"}.get(severity, "⚪")
                print(f"  {emoji} {severity.capitalize()}: {count}")
        
        print("\n📋 By Category:")
        for category, count in analysis_result['by_category'].items():
            if count > 0:
                print(f"  • {category}: {count}")
        
        print("\n🔧 By Tool:")
        for tool, count in analysis_result['by_tool'].items():
            if count > 0:
                print(f"  • {tool}: {count}")
        
        if analysis_result['summary']['critical_files']:
            print("\n⚠️  Files with Most Issues:")
            for file_path in analysis_result['summary']['critical_files'][:5]:
                count = analysis_result['by_file'][file_path]
                print(f"  • {file_path}: {count} issues")
        
        print("\n" + "="*60)


def main():
    """Main entry point for static analysis."""
    parser = argparse.ArgumentParser(description='Static Analysis for Python Cellframe SDK')
    parser.add_argument('--paths', nargs='+', default=['CellFrame/', 'src/', 'tests/'],
                       help='Paths to analyze')
    parser.add_argument('--tools', nargs='+', 
                       choices=['pylint', 'flake8', 'mypy', 'bandit', 'vulture', 'radon'],
                       default=['pylint', 'flake8', 'mypy', 'bandit'],
                       help='Tools to run')
    parser.add_argument('--output', '-o', help='Output file for report')
    parser.add_argument('--quiet', '-q', action='store_true', help='Suppress progress output')
    
    args = parser.parse_args()
    
    analyzer = StaticAnalyzer()
    
    # Run analysis
    result = analyzer.analyze_project(
        target_paths=args.paths,
        tools=args.tools
    )
    
    # Print summary unless quiet
    if not args.quiet:
        analyzer.print_summary(result)
    
    # Save report
    analyzer.save_report(result, args.output)
    
    # Exit with non-zero if high-severity issues found
    high_severity_count = result['by_severity'].get('high', 0)
    if high_severity_count > 0:
        print(f"\n❌ Found {high_severity_count} high-severity issues!")
        sys.exit(1)
    else:
        print(f"\n✅ No high-severity issues found!")
        sys.exit(0)


if __name__ == '__main__':
    main()