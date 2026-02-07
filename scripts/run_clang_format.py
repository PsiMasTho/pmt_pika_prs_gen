#!/usr/bin/env python3

import os
import sys
import subprocess
from pathlib import Path

# Configurable parameters
DIRECTORIES = ["src", "examples"]
EXTENSIONS = {".cpp", ".hpp"}
DEFAULT_CLANG_FORMAT = "clang-format-16"

def find_project_root():
    return Path(__file__).resolve().parent.parent

def find_files(root, directories, extensions):
    files = []
    for directory in directories:
        dir_path = root / directory
        if not dir_path.exists():
            continue
        for path in dir_path.rglob("*"):
            if path.suffix in extensions and path.is_file():
                files.append(str(path))
    return files

def main():
    project_root = find_project_root()
    os.chdir(project_root)

    clang_format = os.environ.get("CLANG_FORMAT", DEFAULT_CLANG_FORMAT)
    if len(sys.argv) > 1:
        clang_format = sys.argv[1]

    files = find_files(project_root, DIRECTORIES, EXTENSIONS)
    if not files:
        print("No files found to format.")
        sys.exit(0)

    print(f"Formatting {len(files)} files with {clang_format}...")

    try:
        result = subprocess.run(
            [clang_format, "-i", "-style=file", "--verbose"] + files,
            check=True,
            capture_output=True,
            text=True
        )
        print(result.stdout)
        print("clang-format completed successfully.")
    except subprocess.CalledProcessError as e:
        print("clang-format failed:")
        print(e.stderr)
        sys.exit(e.returncode)
    except FileNotFoundError:
        print(f"Error: {clang_format} not found. Please install it or set CLANG_FORMAT env variable.")
        sys.exit(1)

if __name__ == "__main__":
    main()