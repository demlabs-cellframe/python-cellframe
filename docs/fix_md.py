import os
import sys
import re


def fix_markdown_files(directory):
    for filename in os.listdir(directory):
        if filename.endswith(".md"):
            filepath = os.path.join(directory, filename)
            with open(filepath, "r") as file:
                content = file.read()

            # Fix Return type: to italic
            content = re.sub(r"\* \*\*Return type:\*\*\s+(.+?)\n(?=\n####|\n##|\n###)",
                             r"* **Return type:** *\1*\n",
                             content, flags=re.DOTALL)

            # Fix Returns: to italic
            content = re.sub(r"\* \*\*Returns:\*\*\s+(.*?)\n(?=\n\*)",
                             r"* **Returns:** \1",
                             content, flags=re.DOTALL)


            # Fix Type: to italic
            content = re.sub(r"\* \*\*Type:\*\*\s+(.*?)(?=\s*##+|$)",
                             r"* **Type:** *\1*\n\n",
                             content, flags=re.DOTALL)

            # Add <#### Attributes:
            content = re.sub(r"(### \*class\*.+?\n\n)(.+?)\n(#### .+?)(\n\n#### |$)",
                             r"\1\2\n\n#### Attributes:\n\n\3\n\n",
                             content, flags=re.DOTALL)


            # Delete  <Bases: `object`>
            pattern_bases = r'^Bases: `object`$'
            content = re.sub(pattern_bases, r'', content, flags=re.MULTILINE)

            # Add <#### Methods:>  работает

            def add_methods(content):
                content_lines = content.split("\n")
                lines_to_modify = []

                for i, line in enumerate(content_lines):
                    if line.startswith("#### \_\_init_\_"):
                        lines_to_modify.append(i)

                for i in reversed(lines_to_modify):  # Проходим в обратном порядке, чтобы не изменять позиции а то Methods улетает в небеса
                    content_lines.insert(i, "")
                    content_lines.insert(i, "#### Methods:")

                modified_content = "\n".join(content_lines)
                return modified_content

            content = add_methods(content)

            # Header logic

            def replace_headers_attributes(content):
                content_lines = content.split("\n")
                inside_section = False

                for i, line in enumerate(content_lines):
                    if line.strip() == "#### Attributes:":
                        inside_section = True
                    elif line.strip() == "#### Methods:":
                        inside_section = False
                    elif inside_section and line.startswith("#### "):
                        content_lines[i] = "#" + line

                content = "\n".join(content_lines)
                return content

            content = replace_headers_attributes(content)

            def replace_headers_methods(content):
                content_lines = content.split("\n")
                inside_section = False

                for i, line in enumerate(content_lines):
                    if line.strip() == "#### Methods:":
                        inside_section = True
                    elif line.startswith("### "):
                        inside_section = False
                    elif inside_section and line.startswith("#### "):
                        content_lines[i] = "#" + line

                content = "\n".join(content_lines)
                return content

            content = replace_headers_methods(content)


            # Delete reapeted empty strings

            content = re.sub(r"(\n\s*\n){2,}", r"\n\n", content)

            # Rewrite
            with open(filepath, "w") as file:
                file.write(content)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        directory = sys.argv[1]
    # directory = "../pycfhelpers_doc/_build/markdown"

    fix_markdown_files(directory)
