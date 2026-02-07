#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path
import subprocess


def main() -> None:
    repo_root = Path(__file__).resolve().parents[1]
    example_dir = repo_root / "examples" / "example_2"

    cmd_1 = [
        "pmt_pika_prs_gen_cli",
        "--start-rules", "round",
        "--input-grammar", str(example_dir / "round.pika"), str(example_dir / "square.pika"), str(example_dir / "shared.pika"),
        "--header-include-path", "round_tables.hpp",
        "--id-constants-include-path", "shared_id_constants-inl.hpp",
        "--output-header", str(example_dir / "round_tables.hpp"),
        "--header-skel", str(repo_root / "skel" / "cpp" / "pika_tables-skel.hpp"),
        "--output-source", str(example_dir / "round_tables.cpp"),
        "--source-skel", str(repo_root / "skel" / "cpp" / "pika_tables-skel.cpp"),
        "--class-name", "RoundTables",
        "--namespace-name", "example",

    ]

    cmd_2 = [
        "pmt_pika_prs_gen_cli",
        "--start-rules", "square",
        "--input-grammar", str(example_dir / "round.pika"), str(example_dir / "square.pika"), str(example_dir / "shared.pika"),
        "--header-include-path", "square_tables.hpp",
        "--id-constants-include-path", "shared_id_constants-inl.hpp",
        "--output-header", str(example_dir / "square_tables.hpp"),
        "--header-skel", str(repo_root / "skel" / "cpp" / "pika_tables-skel.hpp"),
        "--output-source", str(example_dir / "square_tables.cpp"),
        "--source-skel", str(repo_root / "skel" / "cpp" / "pika_tables-skel.cpp"),
        "--class-name", "SquareTables",
        "--namespace-name", "example",
    ]

    cmd_3 = [
        "pmt_pika_prs_gen_cli",
        "--start-rules", "round", "square",
        "--input-grammar", str(example_dir / "round.pika"), str(example_dir / "square.pika"), str(example_dir / "shared.pika"),
        "--output-id-strings", str(example_dir / "shared_id_strings-inl.hpp"),
        "--output-id-constants", str(example_dir / "shared_id_constants-inl.hpp"),
    ]

    subprocess.run(cmd_1, check=True)
    subprocess.run(cmd_2, check=True)
    subprocess.run(cmd_3, check=True)


if __name__ == "__main__":
    main()
