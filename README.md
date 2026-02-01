Pika Parser Generator

Overview
- C++ parser generator for `.pika` grammars. The CLI loads a grammar and emits a `PikaProgram` plus ID string/constant tables.
- `src/pmt/builder/` contains the emitters and CLI, `src/pmt/meta/` contains the self-hosted grammar and helpers, and `src/pmt/rt/` is the runtime (AST + parser).
- Optional outputs include a clauses dump and a terminal graph dotfile for debugging.

Generate the meta grammar
- Run `scripts/gen_meta.py` to regenerate the meta parser sources in `src/pmt/meta/`.
- The script embeds all arguments and resolves paths relative to the repo root, so it works from any CWD.
- It expects `pika_parser_generator_cli` to be available in your `PATH` (build/install the CLI first).
- This doesn't need to be run unless `pmt/meta/grammar.pika` is modified as the repo already includes generated sources.

Skeletons
- `make install-skel` installs only the `skel/` files.
- By default, the CLI uses the installed skeleton root (`${CMAKE_INSTALL_PREFIX}/share/pika_parser_generator/skel`).
- Override skeleton location at runtime with `--skel-dir /path/to/skel`.

VS Code syntax highlighting for .pika
- A minimal local extension lives in `pika-vscode/`.
- Manual install (no packaging):
  - `mkdir -p ~/.vscode/extensions/pika-lang-0.0.1`
  - `cp -R pika-vscode/* ~/.vscode/extensions/pika-lang-0.0.1/`
  - Reload VS Code (`Developer: Reload Window`).
