#!/usr/bin/env python3
import os
from datetime import datetime

# Configuration
DOC_DIR = os.path.dirname(os.path.abspath(__file__))
SECTIONS = [
    "quickstart",
    "overview",
    "cookbook",
    "troubleshooting",
    "glossary",
    "appendix",
    "api"
]
LANGUAGES = ["en", "ru"]
OUTPUT_FILES = {
    "en": "Python_Cellframe_SDK_Guide_EN.md",
    "ru": "Python_Cellframe_SDK_Guide_RU.md"
}

HEADER_TEMPLATE = """# Python-Cellframe SDK Documentation ({lang})
Generated on: {date}

[TOC]

"""

def get_file_title(filepath):
    """Extracts title from the first line of markdown file."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            first_line = f.readline().strip()
            if first_line.startswith('# '):
                return first_line[2:]
    except Exception:
        pass
    return os.path.basename(filepath)

def build_docs(lang):
    output_file = os.path.join(DOC_DIR, OUTPUT_FILES[lang])
    print(f"Building {output_file}...")
    
    content = []
    content.append(HEADER_TEMPLATE.format(lang=lang.upper(), date=datetime.now().strftime("%Y-%m-%d")))
    
    for section in SECTIONS:
        # New structure: doc/{lang}/{section}/
        section_dir = os.path.join(DOC_DIR, lang, section)
        if not os.path.exists(section_dir):
            print(f"  Warning: Section directory not found: {section_dir}")
            continue
            
        # Capitalize section name for header
        section_title = section.capitalize()
        content.append(f"\n## {section_title}\n")
        
        # Get all .md files
        files = sorted([f for f in os.listdir(section_dir) if f.endswith('.md')])
        
        if not files:
            content.append(f"*(No documentation available for this section yet)*\n")
            continue

        for filename in files:
            filepath = os.path.join(section_dir, filename)
            print(f"  Processing {filepath}...")
            
            try:
                with open(filepath, 'r', encoding='utf-8') as f:
                    file_content = f.read()
                    # Ensure header level adjustment if needed (optional, simplistic here)
                    # If files start with # Title, we might want to change them to ### Title to fit under ## Section
                    
                    # Simple heuristic: if it starts with #, demote it to ###, etc? 
                    # For now, just appending.
                    
                    content.append(f"\n{file_content}\n")
                    content.append("\n---\n")
            except Exception as e:
                print(f"  Error reading {filepath}: {e}")

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("\n".join(content))
    
    print(f"Done: {output_file}")

def main():
    for lang in LANGUAGES:
        build_docs(lang)

if __name__ == "__main__":
    main()

