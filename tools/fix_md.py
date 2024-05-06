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

            # Delete ## Module contents

            pattern_bases = r'## Module contents$'
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

            '''def fix_param_rais_hierarchy(content):
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

            content = fix_param_rais_hierarchy(content)'''

            # Removing unnecessary aphostrophes in "Bases"

            def del_aphostrophes(content):
                content_lines = content.split("\n")
                modified_lines = []
                for line in content_lines:
                    if line.startswith("Bases"):
                        line = line.replace("`", "")
                    modified_lines.append(line)
                modified_content = "\n".join(modified_lines)
                return modified_content

            content = del_aphostrophes(content)

            # A blanck for correcting links, creating an ID
            # for each class and writing to the dictionary
            class_dict = {}

            def add_class_ids(content):
                content_lines = content.split("\n")
                modified_lines = []

                for line in content_lines:
                    if line.startswith("### *class* "):
                        class_name = line.split('(')[0]
                        class_name = class_name.strip()

                        class_id = class_name.split('.')[-1]

                        class_id = f"^{class_id}"
                        line = line + f" {class_id}"
                        class_name = class_name.replace("### *class* ", "")
                        class_dict[class_name] = class_id
                    modified_lines.append(line)

                modified_content = "\n".join(modified_lines)
                return modified_content, class_dict

            content, class_dict = add_class_ids(content)

            # Fix links with corresponding parametr
            # from the class dictionary

            def fix_links(content, link_dict):
                for old_link, new_link_id in link_dict.items():
                    link_name = new_link_id.split('^')[-1]

                    if 'List' in content:
                        content = content.replace(f"List[[{link_name}](#{old_link})]",
                                                  f"List\[[[#{new_link_id}|{link_name}]]]")
                    if 'list' in content:
                        content = content.replace(f"list[[{link_name}](#{old_link})]",
                                                  f"list\[[[#{new_link_id}|{link_name}]]]")
                    content = content.replace(f"[{link_name}](#{old_link})",
                                              f"[[#{new_link_id}|{link_name}]]")
                    content = content.replace(f"[*{link_name}*](#{old_link})",
                                              f"[[#{new_link_id}|{link_name}]]")
                return content

            content = fix_links(content, class_dict)

            def fix_union_bra(content):
                content_lines = content.split("\n")
                modified_lines = []
                for line in content_lines:
                    if "*Union* *[**" in line:
                        line = line.replace("*Union* *[**", "*Union* *[* *")
                    elif "Union[" in line:
                        line = line.replace("Union[", "Union\[")

                    modified_lines.append(line)
                modified_content = "\n".join(modified_lines)
                return modified_content

            content = fix_union_bra(content)

            def fix_type_t(content):
                content_lines = content.split("\n")
                modified_lines = []
                for line in content_lines:
                    if "*Type* *[* *T* *]*" in line:
                        line = line.replace("*Type* *[* *T* *]*", "*Type [T]*")
                    elif "*Type* *[**T* *]*" in line:
                        line = line.replace("*Type* *[**T* *]*",  "*Type [T]*")
                    modified_lines.append(line)
                modified_content = "\n".join(modified_lines)
                return modified_content

            content = fix_type_t(content)

            # Add "---" divider

            def add_divider(content):
                content_lines = content.split("\n")
                lines_to_modify = []
                for i, line in enumerate(content_lines):
                    if "package" in line.lower() or "module" in line.lower():
                        lines_to_modify.append(i)

                for i in reversed(lines_to_modify):
                    if i + 1 < len(content_lines):
                        content_lines.insert(i + 1, "---")
                    else:
                        content_lines.append("---")  # добавляем "---" в конец файла, если строка с package/module была последней

                modified_content = "\n".join(content_lines)
                return modified_content

            content = add_divider(content)

            # Rewrite

            with open(filepath, "w", encoding='utf-8') as file:
                file.write(content)



if __name__ == "__main__":
    if len(sys.argv) > 1:
        directory = sys.argv[1]
    # directory = "../pycfhelpers_doc/_build/markdown"
        print(directory)
    fix_markdown_files(directory)
