#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path
import subprocess


def main() -> None:
    repo_root = Path(__file__).resolve().parents[1]
    example_dir = repo_root / "examples" / "example_1"

    cmd = [
        "pmt_pika_prs_gen_cli",
        "--start-rules", "S",
        "--input-grammar", str(example_dir / "grammar.pika"),
        "--header-include-path", "example_tables.hpp",
        "--id-constants-include-path", "id_constants-inl.hpp",
        "--output-header", str(example_dir / "example_tables.hpp"),
        "--header-skel", str(repo_root / "skel" / "cpp" / "pika_tables-skel.hpp"),
        "--output-source", str(example_dir / "example_tables.cpp"),
        "--source-skel", str(repo_root / "skel" / "cpp" / "pika_tables-skel.cpp"),
        "--class-name", "ExampleTables",
        "--namespace-name", "example",
        "--output-id-strings", str(example_dir / "id_strings-inl.hpp"),
        "--output-id-constants", str(example_dir / "id_constants-inl.hpp"),
    ]

    subprocess.run(cmd, check=True)


if __name__ == "__main__":
    main()
