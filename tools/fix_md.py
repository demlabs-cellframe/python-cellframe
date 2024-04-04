import os
import sys
import re


def fix_markdown_files(directory):
    for filename in os.listdir(directory):
        if filename.endswith(".md"):
            filepath = os.path.join(directory, filename)
            with open(filepath, "r+", encoding='utf-8') as file:
                content = file.read()

            # Fix Return type: to italic

            content = re.sub(r"\* \*\*Return type:\*\*\s+(.+?)\n(?=\n####|\n##|\n###)",
                             r"* **Return type:** *\1*\n",
                             content, flags=re.DOTALL)

            # Fix Returns: to italic

            content = re.sub(r"\* \*\*Returns:\*\*\s+(.*?)\n(?=\n\*)",
                             r"* **Returns:** \1",
                             content, flags=re.DOTALL)

            # Fix Yelds: to italic

            content = re.sub(r"\* \*\*Yelds:\*\*\s+(.*?)\n(?=\n\*)",
                             r"* **Yelds:** \1",
                             content, flags=re.DOTALL)

            # Fix Type: to italic

            content = re.sub(r"\* \*\*Type:\*\*\s+(.*?)(?=\s*##+|$)",
                             r"* **Type:** *\1*",
                             content, flags=re.DOTALL)

            # Delete  <Bases: `object`>

            pattern_bases = r'^Bases: `object`$'
            content = re.sub(pattern_bases, r'', content, flags=re.MULTILINE)

            # Add <#### Methods:>

            def add_methods(content):
                content_lines = content.split("\n")
                lines_to_modify = []
                inside_class = False
                for i, line in enumerate(content_lines):
                    if line.startswith("### *class* "):
                        inside_class = True
                    elif line.startswith("####") and line.count('(') > 0 and inside_class:
                        lines_to_modify.append(i)
                        inside_class = False
                    else:
                        continue

                for i in reversed(lines_to_modify):
                    content_lines.insert(i, "")
                    content_lines.insert(i, "#### Methods:")

                modified_content = "\n".join(content_lines)
                return modified_content

            content = add_methods(content)

            # Add <#### Attributes:

            content = re.sub(r"(### \*class\*.+?\n\n)(.+?)(?=\n\n#### Methods:|\n\n####)",
                             r"\1\2\n\n#### Attributes:\n\n",
                             content,
                             flags=re.IGNORECASE | re.DOTALL
                             )

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

            # Delete <#### Attributes:> where there are none

            def delete_excess_attributes(content):
                content_lines = content.split("\n")
                line_index_to_del = []

                for i, line in enumerate(content_lines):
                    if line.strip() == "#### Attributes:":
                        if (content_lines[i+1] == r'' and
                            content_lines[i+2].startswith("#### Methods:")):
                            line_index_to_del.append(i)

                for index in reversed(line_index_to_del):
                    del content_lines[index]

                content = "\n".join(content_lines)
                return content

            content = delete_excess_attributes(content)

            # Fix parameters and raises hierarchy

            def fix_param_rais_hierarchy(content):
                content_lines = content.split("\n")

                for i, line in enumerate(content_lines):
                    if line.startswith("* **Parameters:**"):
                        if content_lines[i+3].startswith("##"):
                            content_lines[i+1] = "\t*" + content_lines[i+1][1:]
                        elif content_lines[i+2].startswith("* "):
                            content_lines[i+1] = "\t*" + content_lines[i+1][1:]
                        elif content_lines[i+2].startswith("##"):
                            content_lines[i+1] = "\t*" + content_lines[i+1][1:]
                    elif line.startswith("* **Raises:**"):
                        if content_lines[i+3].startswith("##"):
                            content_lines[i+1] = "\t*" + content_lines[i+1][1:]

                content = "\n".join(content_lines)
                return content

            content = fix_param_rais_hierarchy(content)

            # Rewrite

            with open(filepath, "w", encoding='utf-8') as file:
                file.write(content)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        directory = sys.argv[1]
    # directory = "../pycfhelpers_doc/_build/markdown"
        print(directory)
    fix_markdown_files(directory)
