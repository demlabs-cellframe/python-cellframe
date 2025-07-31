#!/usr/bin/env python3
"""
🔧 Automatic Code Fixer for Python Cellframe SDK
Automated fixes for common code issues.
"""

import os
import re
import json
import argparse
import subprocess
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple
from datetime import datetime
import tempfile
import shutil


class AutoFixer:
    """Automatic code fixer with safe application of common fixes."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.fixes_applied = []
        self.backup_dir = None
        
    def create_backup(self, files: List[str]) -> str:
        """Create backup of files before modification."""
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        backup_dir = self.project_root / f".backup_{timestamp}"
        backup_dir.mkdir(exist_ok=True)
        
        for file_path in files:
            src_path = Path(file_path)
            if src_path.exists():
                # Maintain directory structure in backup
                rel_path = src_path.relative_to(self.project_root)
                backup_path = backup_dir / rel_path
                backup_path.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(src_path, backup_path)
        
        self.backup_dir = str(backup_dir)
        print(f"📁 Backup created: {backup_dir}")
        return str(backup_dir)
    
    def fix_unused_imports(self, file_path: str) -> bool:
        """Remove unused imports from a file."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            lines = content.split('\n')
            modified_lines = []
            imports_to_check = []
            
            # First pass: identify import lines
            for i, line in enumerate(lines):
                if (line.strip().startswith('import ') or 
                    line.strip().startswith('from ') and ' import ' in line):
                    
                    # Extract imported names
                    if line.strip().startswith('import '):
                        imported = line.strip()[7:].split(',')
                        imported = [imp.strip().split(' as ')[0] for imp in imported]
                    elif line.strip().startswith('from '):
                        parts = line.strip().split(' import ')
                        if len(parts) == 2:
                            imported = parts[1].split(',')
                            imported = [imp.strip().split(' as ')[0] for imp in imported]
                        else:
                            imported = []
                    
                    imports_to_check.append({
                        'line_number': i,
                        'line': line,
                        'imported': imported
                    })
            
            # Second pass: check if imports are used
            content_without_imports = '\n'.join([
                line for i, line in enumerate(lines) 
                if i not in [imp['line_number'] for imp in imports_to_check]
            ])
            
            for import_info in imports_to_check:
                line_number = import_info['line_number']
                imported_names = import_info['imported']
                
                # Check if any imported name is used
                used = False
                for name in imported_names:
                    if name and (name in content_without_imports or 
                                f".{name}" in content_without_imports or
                                f"({name}" in content_without_imports):
                        used = True
                        break
                
                # Keep the import if it's used
                if used or 'import *' in import_info['line']:
                    modified_lines.append((line_number, import_info['line']))
                else:
                    print(f"  🗑️  Removing unused import at line {line_number + 1}: {import_info['line'].strip()}")
            
            # Reconstruct file
            final_lines = []
            import_lines_kept = {line_num: line for line_num, line in modified_lines}
            
            for i, line in enumerate(lines):
                if i in import_lines_kept:
                    final_lines.append(import_lines_kept[i])
                elif i not in [imp['line_number'] for imp in imports_to_check]:
                    final_lines.append(line)
                # Skip removed import lines
            
            # Write back if changes were made
            new_content = '\n'.join(final_lines)
            if new_content != content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(new_content)
                return True
                
        except Exception as e:
            print(f"⚠️  Error fixing imports in {file_path}: {e}")
        
        return False
    
    def fix_unused_variables(self, file_path: str) -> bool:
        """Fix unused variables by adding underscore prefix."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            lines = content.split('\n')
            modified = False
            
            for i, line in enumerate(lines):
                # Simple heuristic for unused variables
                # Look for variable assignments that are never referenced
                if '=' in line and not line.strip().startswith('#'):
                    # Extract variable name from assignment
                    assignment_match = re.match(r'^(\s*)([a-zA-Z_][a-zA-Z0-9_]*)\s*=', line)
                    if assignment_match:
                        indent = assignment_match.group(1)
                        var_name = assignment_match.group(2)
                        
                        # Skip if already prefixed with underscore
                        if var_name.startswith('_'):
                            continue
                        
                        # Check if variable is used later
                        rest_of_file = '\n'.join(lines[i+1:])
                        if var_name not in rest_of_file:
                            # Replace with underscore-prefixed version
                            new_line = line.replace(f'{var_name} =', f'_{var_name} =')
                            lines[i] = new_line
                            modified = True
                            print(f"  🔧 Fixed unused variable at line {i + 1}: {var_name} -> _{var_name}")
            
            if modified:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write('\n'.join(lines))
                return True
                
        except Exception as e:
            print(f"⚠️  Error fixing variables in {file_path}: {e}")
        
        return False
    
    def fix_line_length(self, file_path: str, max_length: int = 100) -> bool:
        """Fix long lines by breaking them appropriately."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            lines = content.split('\n')
            modified = False
            
            for i, line in enumerate(lines):
                if len(line) > max_length and not line.strip().startswith('#'):
                    # Try to break long lines
                    if ',' in line and '(' in line and ')' in line:
                        # Function call or list - break at commas
                        indent = len(line) - len(line.lstrip())
                        new_lines = self._break_at_commas(line, indent, max_length)
                        if len(new_lines) > 1:
                            lines[i:i+1] = new_lines
                            modified = True
                            print(f"  📏 Fixed long line at {i + 1} (was {len(line)} chars)")
                    
                    elif ' and ' in line or ' or ' in line:
                        # Boolean expressions - break at logical operators
                        new_lines = self._break_at_operators(line, max_length)
                        if len(new_lines) > 1:
                            lines[i:i+1] = new_lines
                            modified = True
                            print(f"  📏 Fixed long line at {i + 1} (was {len(line)} chars)")
            
            if modified:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write('\n'.join(lines))
                return True
                
        except Exception as e:
            print(f"⚠️  Error fixing line length in {file_path}: {e}")
        
        return False
    
    def _break_at_commas(self, line: str, indent: int, max_length: int) -> List[str]:
        """Break a line at commas while maintaining proper indentation."""
        if '(' not in line:
            return [line]
        
        # Find the opening parenthesis
        paren_pos = line.find('(')
        prefix = line[:paren_pos + 1]
        suffix_start = line.rfind(')')
        suffix = line[suffix_start:]
        middle = line[paren_pos + 1:suffix_start]
        
        # Split by commas and rejoin with proper indentation
        parts = [part.strip() for part in middle.split(',')]
        if len(parts) <= 1:
            return [line]
        
        new_lines = [prefix]
        base_indent = ' ' * (indent + 4)  # 4 spaces additional indent
        
        for i, part in enumerate(parts):
            if part:  # Skip empty parts
                comma = ',' if i < len(parts) - 1 else ''
                new_lines.append(f"{base_indent}{part}{comma}")
        
        new_lines.append(' ' * indent + suffix)
        
        return new_lines
    
    def _break_at_operators(self, line: str, max_length: int) -> List[str]:
        """Break a line at logical operators."""
        indent = len(line) - len(line.lstrip())
        base_indent = ' ' * indent
        continuation_indent = ' ' * (indent + 4)
        
        # Break at 'and' and 'or'
        for operator in [' and ', ' or ']:
            if operator in line:
                parts = line.split(operator)
                if len(parts) > 1:
                    new_lines = [parts[0] + ' \\']
                    for i, part in enumerate(parts[1:], 1):
                        prefix = continuation_indent if i > 0 else base_indent
                        connector = operator.strip() + ' ' if i < len(parts) - 1 else ''
                        new_lines.append(f"{prefix}{operator.strip()} {part.strip()}")
                    return new_lines
        
        return [line]
    
    def fix_whitespace_issues(self, file_path: str) -> bool:
        """Fix common whitespace issues."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            original_content = content
            
            # Fix multiple blank lines
            content = re.sub(r'\n\s*\n\s*\n+', '\n\n', content)
            
            # Fix trailing whitespace
            lines = content.split('\n')
            lines = [line.rstrip() for line in lines]
            content = '\n'.join(lines)
            
            # Fix spaces around operators
            content = re.sub(r'([a-zA-Z0-9_])\s*=\s*([a-zA-Z0-9_])', r'\1 = \2', content)
            content = re.sub(r'([a-zA-Z0-9_])\s*\+\s*([a-zA-Z0-9_])', r'\1 + \2', content)
            content = re.sub(r'([a-zA-Z0-9_])\s*-\s*([a-zA-Z0-9_])', r'\1 - \2', content)
            
            if content != original_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                print(f"  🧹 Fixed whitespace issues in {file_path}")
                return True
                
        except Exception as e:
            print(f"⚠️  Error fixing whitespace in {file_path}: {e}")
        
        return False
    
    def fix_import_order(self, file_path: str) -> bool:
        """Fix import order according to PEP 8."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            lines = content.split('\n')
            
            # Separate imports from other code
            import_lines = []
            other_lines = []
            import_section_ended = False
            
            for line in lines:
                stripped = line.strip()
                if (stripped.startswith('import ') or 
                    (stripped.startswith('from ') and ' import ' in stripped)) and not import_section_ended:
                    import_lines.append(line)
                else:
                    if stripped and not stripped.startswith('#') and import_lines:
                        import_section_ended = True
                    other_lines.append(line)
            
            if not import_lines:
                return False
            
            # Categorize imports
            stdlib_imports = []
            third_party_imports = []
            local_imports = []
            
            stdlib_modules = {
                'os', 'sys', 'json', 'time', 'datetime', 'pathlib', 'subprocess',
                'tempfile', 'shutil', 'argparse', 'logging', 'unittest', 're',
                'collections', 'itertools', 'functools', 'typing', 'dataclasses',
                'enum', 'abc', 'copy', 'pickle', 'csv', 'urllib', 'http', 'socket'
            }
            
            for import_line in import_lines:
                stripped = import_line.strip()
                if stripped.startswith('from '):
                    module = stripped.split(' import ')[0][5:].split('.')[0]
                elif stripped.startswith('import '):
                    module = stripped[7:].split('.')[0].split(' as ')[0]
                else:
                    continue
                
                if module in stdlib_modules:
                    stdlib_imports.append(import_line)
                elif module.startswith('CellFrame') or module.startswith('.'):
                    local_imports.append(import_line)
                else:
                    third_party_imports.append(import_line)
            
            # Sort within each category
            stdlib_imports.sort()
            third_party_imports.sort()
            local_imports.sort()
            
            # Reconstruct imports with proper spacing
            sorted_imports = []
            if stdlib_imports:
                sorted_imports.extend(stdlib_imports)
                sorted_imports.append('')  # Blank line
            if third_party_imports:
                sorted_imports.extend(third_party_imports)
                sorted_imports.append('')  # Blank line
            if local_imports:
                sorted_imports.extend(local_imports)
                sorted_imports.append('')  # Blank line
            
            # Remove trailing blank line if no other code follows
            if sorted_imports and sorted_imports[-1] == '':
                sorted_imports.pop()
            
            # Reconstruct file
            new_content = '\n'.join(sorted_imports + other_lines)
            
            if new_content != content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(new_content)
                print(f"  📚 Fixed import order in {file_path}")
                return True
                
        except Exception as e:
            print(f"⚠️  Error fixing import order in {file_path}: {e}")
        
        return False
    
    def apply_auto_fixes(self, 
                        file_paths: List[str],
                        fix_types: List[str] = None) -> Dict[str, Any]:
        """Apply automatic fixes to specified files."""
        
        if fix_types is None:
            fix_types = ['unused_imports', 'unused_variables', 'whitespace', 'import_order']
        
        # Create backup
        backup_dir = self.create_backup(file_paths)
        
        fixes_summary = {
            'backup_dir': backup_dir,
            'files_processed': 0,
            'files_modified': 0,
            'fixes_applied': [],
            'errors': []
        }
        
        print(f"\n🔧 Applying automatic fixes...")
        print(f"Fix types: {', '.join(fix_types)}")
        
        for file_path in file_paths:
            if not Path(file_path).exists():
                continue
            
            print(f"\n📄 Processing: {file_path}")
            fixes_summary['files_processed'] += 1
            file_modified = False
            
            try:
                if 'unused_imports' in fix_types:
                    if self.fix_unused_imports(file_path):
                        file_modified = True
                        fixes_summary['fixes_applied'].append({
                            'file': file_path,
                            'fix_type': 'unused_imports'
                        })
                
                if 'unused_variables' in fix_types:
                    if self.fix_unused_variables(file_path):
                        file_modified = True
                        fixes_summary['fixes_applied'].append({
                            'file': file_path,
                            'fix_type': 'unused_variables'
                        })
                
                if 'line_length' in fix_types:
                    if self.fix_line_length(file_path):
                        file_modified = True
                        fixes_summary['fixes_applied'].append({
                            'file': file_path,
                            'fix_type': 'line_length'
                        })
                
                if 'whitespace' in fix_types:
                    if self.fix_whitespace_issues(file_path):
                        file_modified = True
                        fixes_summary['fixes_applied'].append({
                            'file': file_path,
                            'fix_type': 'whitespace'
                        })
                
                if 'import_order' in fix_types:
                    if self.fix_import_order(file_path):
                        file_modified = True
                        fixes_summary['fixes_applied'].append({
                            'file': file_path,
                            'fix_type': 'import_order'
                        })
                
                if file_modified:
                    fixes_summary['files_modified'] += 1
                    print(f"  ✅ Modified: {file_path}")
                else:
                    print(f"  ➡️  No changes needed: {file_path}")
                    
            except Exception as e:
                error_msg = f"Error processing {file_path}: {e}"
                fixes_summary['errors'].append(error_msg)
                print(f"  ❌ {error_msg}")
        
        return fixes_summary
    
    def restore_from_backup(self, backup_dir: str) -> bool:
        """Restore files from backup."""
        try:
            backup_path = Path(backup_dir)
            if not backup_path.exists():
                print(f"❌ Backup directory not found: {backup_dir}")
                return False
            
            for backup_file in backup_path.rglob('*'):
                if backup_file.is_file():
                    rel_path = backup_file.relative_to(backup_path)
                    target_path = self.project_root / rel_path
                    target_path.parent.mkdir(parents=True, exist_ok=True)
                    shutil.copy2(backup_file, target_path)
            
            print(f"✅ Restored files from backup: {backup_dir}")
            return True
            
        except Exception as e:
            print(f"❌ Error restoring from backup: {e}")
            return False


def main():
    """Main entry point for auto fixer."""
    parser = argparse.ArgumentParser(description='Automatic Code Fixer for Python Cellframe SDK')
    parser.add_argument('--paths', nargs='+', required=True, help='Files or directories to fix')
    parser.add_argument('--fixes', nargs='+', 
                       choices=['unused_imports', 'unused_variables', 'line_length', 'whitespace', 'import_order'],
                       default=['unused_imports', 'whitespace', 'import_order'],
                       help='Types of fixes to apply')
    parser.add_argument('--output', '-o', help='Output file for fix report')
    parser.add_argument('--restore', help='Restore from backup directory')
    parser.add_argument('--dry-run', action='store_true', help='Show what would be fixed without making changes')
    
    args = parser.parse_args()
    
    fixer = AutoFixer()
    
    if args.restore:
        # Restore from backup
        success = fixer.restore_from_backup(args.restore)
        return 0 if success else 1
    
    # Collect all Python files from paths
    all_files = []
    for path in args.paths:
        path_obj = Path(path)
        if path_obj.is_file() and path.endswith('.py'):
            all_files.append(str(path_obj))
        elif path_obj.is_dir():
            all_files.extend([str(f) for f in path_obj.rglob('*.py')])
    
    if not all_files:
        print("❌ No Python files found in specified paths")
        return 1
    
    print(f"🔍 Found {len(all_files)} Python files to process")
    
    if args.dry_run:
        print("🔎 DRY RUN MODE - No changes will be made")
        # TODO: Implement dry run logic
        return 0
    
    # Apply fixes
    result = fixer.apply_auto_fixes(all_files, args.fixes)
    
    # Save report
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(result, f, indent=2)
        print(f"\n📝 Fix report saved to: {args.output}")
    
    # Print summary
    print(f"\n📊 FIX SUMMARY:")
    print(f"  Files processed: {result['files_processed']}")
    print(f"  Files modified: {result['files_modified']}")
    print(f"  Total fixes applied: {len(result['fixes_applied'])}")
    print(f"  Errors: {len(result['errors'])}")
    
    if result['errors']:
        print(f"\n❌ Errors occurred:")
        for error in result['errors']:
            print(f"  • {error}")
    
    print(f"\n💾 Backup created at: {result['backup_dir']}")
    print(f"   Use --restore {result['backup_dir']} to undo changes")
    
    return 0


if __name__ == '__main__':
    exit(main())