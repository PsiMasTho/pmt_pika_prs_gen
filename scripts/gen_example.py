#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path
import subprocess


def main() -> None:
    repo_root = Path(__file__).resolve().parents[1]
    example_dir = repo_root / "example"

    cmd = [
        "pmt_pika_prs_gen_cli",
        "--input-grammar-file", str(example_dir / "grammar.pika"),
        "--pika-tables-header-include-filename", "example_tables.hpp",
        "--pika-tables-output-header-file", str(example_dir / "example_tables.hpp"),
        "--pika-tables-header-skel-file", str(repo_root / "skel" / "pmt" / "builder" / "pika_tables-skel.hpp"),
        "--pika-tables-output-source-file", str(example_dir / "example_tables.cpp"),
        "--pika-tables-source-skel-file", str(repo_root / "skel" / "pmt" / "builder" / "pika_tables-skel.cpp"),
        "--pika-tables-class-name", "ExampleTables",
        "--pika-tables-namespace-name", "example",
        "--id-strings-output-file", str(example_dir / "id_strings-inl.hpp"),
        "--id-constants-output-file", str(example_dir / "id_constants-inl.hpp"),
    ]

    subprocess.run(cmd, check=True)


if __name__ == "__main__":
    main()
