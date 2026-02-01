#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path
import subprocess


def main() -> None:
    repo_root = Path(__file__).resolve().parents[1]
    meta_dir = repo_root / "src" / "pmt" / "meta"

    cmd = [
        "pika_parser_generator_cli",
        "--input-grammar-file",
        str(meta_dir / "grammar.pika"),
        "--pika-program-header-include-filename",
        "pmt/meta/pika_program.hpp",
        "--pika-program-output-header-file",
        str(meta_dir / "pika_program.hpp"),
        "--pika-program-header-skel-file",
        str(repo_root / "skel" / "pmt" / "builder" / "pika_program-skel.hpp"),
        "--pika-program-output-source-file",
        str(meta_dir / "pika_program.cpp"),
        "--pika-program-source-skel-file",
        str(repo_root / "skel" / "pmt" / "builder" / "pika_program-skel.cpp"),
        "--pika-program-class-name",
        "PikaProgram",
        "--pika-program-namespace-name",
        "pmt::meta",
        "--id-strings-output-file",
        str(meta_dir / "id_strings-inl.hpp"),
        "--id-constants-output-file",
        str(meta_dir / "id_constants-inl.hpp"),
    ]

    subprocess.run(cmd, check=True)


if __name__ == "__main__":
    main()
