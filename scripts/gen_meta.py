#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path
import subprocess


def main() -> None:
    repo_root = Path(__file__).resolve().parents[1]
    meta_dir = repo_root / "src" / "pmt" / "meta"

    cmd = [
        "pmt_pika_prs_gen_cli",
        "--start-rules", "grammar",
        "--input-grammar", str(meta_dir / "grammar.pika"),
        "--header-include-path", "pmt/meta/pika_tables.hpp",
        "--id-constants-include-path", "pmt/meta/id_constants-inl.hpp",
        "--output-header", str(meta_dir / "pika_tables.hpp"),
        "--header-skel", str(repo_root / "skel" / "cpp" / "pika_tables-skel.hpp"),
        "--output-source", str(meta_dir / "pika_tables.cpp"),
        "--source-skel", str(repo_root / "skel" / "cpp" / "pika_tables-skel.cpp"),
        "--class-name", "PikaTables",
        "--namespace-name", "pmt::meta",
        "--output-id-strings", str(meta_dir / "id_strings-inl.hpp"),
        "--output-id-constants", str(meta_dir / "id_constants-inl.hpp"),
    ]

    subprocess.run(cmd, check=True)

if __name__ == "__main__":
    main()