#!/usr/bin/env python3
"""
Script to split cellframe_ledger.c into modular files
"""
import re
import os

def split_ledger_file():
    src_file = 'src/cellframe_ledger.c'
    include_file = 'src/include/cellframe_ledger_internal.h'
    
    # Read source file
    with open(src_file, 'r') as f:
        lines = f.readlines()
    
    # Map sections to module names
    section_to_module = {
        'LIFECYCLE OPERATIONS': 'lifecycle',
        'TOKEN TICKER OPERATIONS': 'lifecycle',
        'TRANSACTION OPERATIONS': 'tx',
        'TOKEN OPERATIONS': 'token',
        'TOKEN EMISSION OPERATIONS': 'token',
        'TOKEN INFO AND QUERY OPERATIONS': 'token',
        'TOKEN AND BALANCE INFO OPERATIONS (JSON)': 'balance',
        'BALANCE & FIND OPERATIONS': 'balance',
        'TX LISTS & OPERATIONS': 'tx_lists',
        'TOKEN & EMISSION OPERATIONS': 'token',
        'DATUM ITERATOR OPERATIONS': 'utils',
        'CONDITIONAL OUTPUTS OPERATIONS': 'cond',
        'CALLBACKS & NOTIFICATIONS': 'callbacks',
        'CHAIN PURGE & MISC OPERATIONS': 'utils',
        'DECREE OPERATIONS': 'decree',
        'ANCHOR OPERATIONS': 'anchor',
        'UTILITY & SERVICE FUNCTIONS': 'utils',
        'CALLBACKS & SPECIAL OPERATIONS': 'callbacks',
        'TRACKER & COLOR OPERATIONS': 'utils',
        'AGGREGATION & DECREE OPERATIONS': 'utils',
        'EVENT OPERATIONS': 'event',
    }
    
    # Find section boundaries and module initialization
    sections = []
    current_module = None
    current_start = None
    
    for i, line in enumerate(lines):
        if '// =============================================================================' in line:
            # Look ahead for section name
            if i + 1 < len(lines):
                next_line = lines[i+1].strip()
                if next_line.startswith('//'):
                    section_name = next_line.replace('//', '').strip()
                    if section_name in section_to_module:
                        if current_module:
                            sections.append((current_module, current_start, i))
                        current_module = section_to_module[section_name]
                        current_start = i
                    elif 'MODULE INITIALIZATION' in section_name:
                        if current_module:
                            sections.append((current_module, current_start, i))
                        # Module init starts here
                        sections.append(('init', i, len(lines)))
                        break
    
    if current_module and sections[-1][0] != 'init':
        sections.append((current_module, current_start, len(lines)))
    
    # Group by module
    modules = {}
    for module, start, end in sections:
        if module not in modules:
            modules[module] = []
        modules[module].append((start, end))
    
    # Extract header (first 13 lines)
    header_lines = lines[:13]
    
    # Create module files
    for module_name, ranges in sorted(modules.items()):
        if module_name == 'init':
            continue  # Skip init, we'll handle it separately
            
        output_file = f'src/cellframe_ledger_{module_name}.c'
        print(f"Creating {output_file}...")
        
        with open(output_file, 'w') as f:
            # Write header
            f.write(''.join(header_lines))
            f.write('\n')
            
            # Write all sections for this module
            for start, end in ranges:
                f.write(''.join(lines[start:end]))
            
            # Add get_methods function stub (will be filled manually)
            f.write(f'\n// Get method definitions for {module_name} module\n')
            f.write('PyMethodDef* cellframe_ledger_{}_get_methods(void) {{\n'.format(module_name))
            f.write('    // TODO: Implement method definitions\n')
            f.write('    static PyMethodDef methods[] = {\n')
            f.write('        {NULL, NULL, 0, NULL}  // Sentinel\n')
            f.write('    };\n')
            f.write('    return methods;\n')
            f.write('}\n')
    
    print("\nDone! Now you need to:")
    print("1. Fill in get_methods() functions in each module")
    print("2. Update cellframe_ledger_init.c to call all get_methods()")
    print("3. Update CMakeLists.txt to include all new .c files")

if __name__ == '__main__':
    split_ledger_file()

