Pika Parser Generator

Overview
- C++ parser generator for `.pika` grammars. The CLI loads a grammar and emits a `PikaTables` plus ID string/constant tables.
- `src/pmt/builder/` contains the emitters and CLI, `src/pmt/meta/` contains the self-hosted grammar and helpers, and `src/pmt/rt/` is the runtime (AST + parser).
- Optional outputs include a clauses dump and a terminal graph dotfile for debugging.

Generate the meta grammar
- Run `scripts/gen_meta.py` to regenerate the meta parser sources in `src/pmt/meta/`.
- The script embeds all arguments and resolves paths relative to the repo root, so it works from any CWD.
- It expects `pika_parser_generator_cli` to be available in your `PATH` (build/install the CLI first).
- This doesn't need to be run unless `pmt/meta/grammar.pika` is modified as the repo already includes generated sources.

CLI arguments
- Required
  - `--input-grammar-file`: Path to the `.pika` grammar to compile.
  - `--pika-tables-header-include-filename`: Header include path embedded in the generated `.cpp` (e.g. `pmt/meta/pika_tables.hpp`).
  - `--pika-tables-output-header-file`: Output path for the generated `PikaTables` header.
  - `--pika-tables-output-source-file`: Output path for the generated `PikaTables` source.
  - `--id-strings-output-file`: Output path for the generated ID string table include.
  - `--id-constants-output-file`: Output path for the generated ID constant table include.
- Optional
  - `--input-test-file`: Parse this input with the generated program and print the AST.
  - `--pika-tables-header-skel-file`: Template for the generated `PikaTables` header. Default: `skel/pmt/builder/pika_tables-skel.hpp`.
  - `--pika-tables-source-skel-file`: Template for the generated `PikaTables` source. Default: `skel/pmt/builder/pika_tables-skel.cpp`.
  - `--pika-tables-class-name`: Override the generated class name (default is empty for the template to decide).
  - `--pika-tables-namespace-name`: Override the generated namespace (default is empty for the template to decide).
  - `--id-strings-skel-file`: Template for the ID string table include. Default: `skel/pmt/builder/id_strings-skel.hpp`.
  - `--id-constants-skel-file`: Template for the ID constant table include. Default: `skel/pmt/builder/id_constants-skel.hpp`.
  - `--output-grammar`: Write the normalized grammar to this file.
  - `--output-clauses`: Write a clauses dump to this file.
  - `--output-dotfile`: Write the terminal graph dotfile to this file.
  - `--terminal-graph-skel-file`: Template for the terminal graph dotfile. Default: `skel/pmt/builder/state_machine-skel.dot` (only used when `--output-dotfile` is set).
  - `--skel-dir`: Override the skeleton root directory used for default template lookups.

Skeletons
- `make install-skel` installs only the `skel/` files.
- By default, the CLI uses the installed skeleton root (`${CMAKE_INSTALL_PREFIX}/share/pmt_pika_prs_gen/skel`).
- Override skeleton location at runtime with `--skel-dir /path/to/skel`.
- Skeletons are templates that control the structure of the generated sources. You can edit the defaults in `skel/` or point the CLI at custom templates via the `--*-skel-file` arguments.

VS Code syntax highlighting for .pika
- A minimal local extension lives in `pika-vscode/`.
- Manual install (no packaging):
  - `mkdir -p ~/.vscode/extensions/pika-lang-0.0.1`
  - `cp -R pika-vscode/* ~/.vscode/extensions/pika-lang-0.0.1/`
  - Reload VS Code (`Developer: Reload Window`).
